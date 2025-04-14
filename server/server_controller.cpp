/* server_controller.cpp

   This application acts as the central controller/receiver on the server side.
   It listens for TCP control traffic, UDP stream data, and runs an HTTP server
   for hosts to poll for instructions.

   Features:
     - Configurable TCP and UDP ports (to choose which protocols are enabled via CLI)
     - An embedded HTTP server (using cpp-httplib) serving endpoints in the form "/<stream_id>"
       so that hosts poll their own endpoint for instructions.
     - Per-stream control: each stream is identified by an ID;
       external operators (or the runtime control loop) may toggle a stream “on” or “off.”
       When a stream is off, the HTTP endpoint instructs the host to stop transmitting.
     - Modular design: All networking components run in separate threads.
  
   Build instructions (Linux/Unix):
     1. Download "httplib.h" from https://github.com/yhirose/cpp-httplib and put it in the
        same directory as this file.
     2. Compile with:
           g++ -std=c++11 server_controller.cpp -o server_controller -lpthread
     3. Run:
           ./server_controller [options]

   Command-line options (example):
     --tcpport <port>     (default: 9000)
     --udpport <port>     (default: 8000)
     --httpport <port>    (default: 8080)
     --enable-tcp         Enable TCP listening (default enabled)
     --enable-udp         Enable UDP listening (default enabled)
     --help               Show usage information
*/

#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <map>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <csignal>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <getopt.h>

// Include cpp-httplib (download from https://github.com/yhirose/cpp-httplib)
#include "httplib.h"

#define DEFAULT_TCP_PORT 9000
#define DEFAULT_UDP_PORT 8000
#define DEFAULT_HTTP_PORT 8080

struct StreamInfo {
    std::string streamId;
    std::atomic<bool> active;       // Whether the stream is active (i.e. should be receiving/forwarding)
    std::atomic<bool> viewing;      // True if somebody is viewing the stream
    // You could add a buffer or more fields here (timestamps, last data received, etc.)
    
    StreamInfo() : active(false), viewing(false) {}
    StreamInfo(const std::string &id) : streamId(id), active(false), viewing(false) {}
};

std::map<std::string, StreamInfo> g_streams;
std::mutex g_streamsMutex;

std::atomic<bool> g_shutdown(false);

void tcpReceiver(int tcpPort) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0) {
        std::cerr << "[TCP] Socket creation error." << std::endl;
        return;
    }
    
    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "[TCP] setsockopt error." << std::endl;
        close(server_fd);
        return;
    }
    
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    address.sin_port = htons(tcpPort);
    
    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "[TCP] Bind failed." << std::endl;
        close(server_fd);
        return;
    }
    
    if(listen(server_fd, 10) < 0) {
        std::cerr << "[TCP] Listen failed." << std::endl;
        close(server_fd);
        return;
    }
    
    std::cout << "[TCP] Listening on port " << tcpPort << " ..." << std::endl;
    
    while (!g_shutdown.load()) {
        sockaddr_in clientAddress;
        socklen_t addrLen = sizeof(clientAddress);
        int new_socket = accept(server_fd, (struct sockaddr *)&clientAddress, &addrLen);
        if(new_socket < 0) {
            if(g_shutdown.load()) break;
            std::cerr << "[TCP] Accept failed." << std::endl;
            continue;
        }
        
        // Receive a simple message (for demo purpose, assume message is the stream id and a command)
        char buffer[1024] = {0};
        int bytes_read = read(new_socket, buffer, sizeof(buffer)-1);
        if (bytes_read > 0) {
            std::string msg(buffer);
            std::istringstream iss(msg);
            std::string streamId, command;
            iss >> streamId >> command;
            
            // Lock and update stream state
            {
                std::lock_guard<std::mutex> lock(g_streamsMutex);
                if (g_streams.find(streamId) == g_streams.end()) {
                    g_streams[streamId] = StreamInfo(streamId);
                    std::cout << "[TCP] New stream registered: " << streamId << std::endl;
                }
                // For demo, we interpret command "START" or "STOP"
                if(command == "START") {
                    g_streams[streamId].active = true;
                    std::cout << "[TCP] Stream " << streamId << " set to ACTIVE." << std::endl;
                } else if (command == "STOP") {
                    g_streams[streamId].active = false;
                    std::cout << "[TCP] Stream " << streamId << " set to INACTIVE." << std::endl;
                } else {
                    std::cout << "[TCP] Received unknown command for stream " << streamId << ": " << command << std::endl;
                }
            }
        }
        close(new_socket);
    }
    close(server_fd);
    std::cout << "[TCP] Shutting down TCP Receiver." << std::endl;
}

void udpReceiver(int udpPort) {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0) {
        std::cerr << "[UDP] Socket creation failed." << std::endl;
        return;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(udpPort);

    if(bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "[UDP] Bind failed." << std::endl;
        close(sock_fd);
        return;
    }

    std::cout << "[UDP] Listening on port " << udpPort << " ..." << std::endl;
    char buffer[2048];
    while (!g_shutdown.load()) {
        sockaddr_in senderAddr;
        socklen_t senderLen = sizeof(senderAddr);
        int bytes_received = recvfrom(sock_fd, buffer, sizeof(buffer)-1, MSG_DONTWAIT, (struct sockaddr *)&senderAddr, &senderLen);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::string msg(buffer);
            std::istringstream iss(msg);
            std::string streamId;
            iss >> streamId;
            
            {
                std::lock_guard<std::mutex> lock(g_streamsMutex);
                if (g_streams.find(streamId) == g_streams.end()) {
                    g_streams[streamId] = StreamInfo(streamId);
                    std::cout << "[UDP] New stream registered: " << streamId << std::endl;
                }
                std::cout << "[UDP] Received data for stream " << streamId << ": " << msg.substr(streamId.size()+1) << std::endl;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    close(sock_fd);
    std::cout << "[UDP] Shutting down UDP Receiver." << std::endl;
}

void httpServer(int httpPort) {
    httplib::Server svr;
    
    svr.Get(R"(/(.*))", [](const httplib::Request &req, httplib::Response &res) {
        std::string streamId = req.matches[1];
        std::lock_guard<std::mutex> lock(g_streamsMutex);
        std::string instruction;
        // For simplicity, we return a JSON message with a field "command".
        // If the stream is active and is being viewed, instruct "KEEP"; otherwise "STOP".
        auto it = g_streams.find(streamId);
        if (it != g_streams.end()) {
            if (it->second.active && it->second.viewing) {
                instruction = R"({"command": "KEEP"})";
            } else {
                instruction = R"({"command": "STOP"})";
            }
        } else {
            // Unknown stream returns a default response
            instruction = R"({"command": "UNKNOWN"})";
        }
        res.set_content(instruction, "application/json");
    });
    
    std::cout << "[HTTP] Server listening on port " << httpPort << " ..." << std::endl;
    svr.listen("0.0.0.0", httpPort);
}

void runtimeControlLoop() {
    std::string line;
    std::cout << "Runtime Control: Type 'list' to view streams, 'toggle <streamId>' to toggle viewing state, or 'quit' to exit." << std::endl;
    while (!g_shutdown.load()) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line == "quit") {
            g_shutdown = true;
            break;
        } else if (line == "list") {
            std::lock_guard<std::mutex> lock(g_streamsMutex);
            std::cout << "Current streams:" << std::endl;
            for (const auto &pair : g_streams) {
                std::cout << "  [" << pair.first << "] Active=" << pair.second.active.load() 
                          << ", Viewing=" << pair.second.viewing.load() << std::endl;
            }
        } else if (line.find("toggle") == 0) {
            std::istringstream iss(line);
            std::string cmd, streamId;
            iss >> cmd >> streamId;
            if (!streamId.empty()) {
                std::lock_guard<std::mutex> lock(g_streamsMutex);
                auto it = g_streams.find(streamId);
                if (it != g_streams.end()) {
                    // Toggle the "viewing" flag
                    bool current = it->second.viewing.load();
                    it->second.viewing = !current;
                    std::cout << "Stream [" << streamId << "] viewing state set to " 
                              << (it->second.viewing.load() ? "TRUE" : "FALSE") << std::endl;
                } else {
                    std::cout << "Stream [" << streamId << "] not found." << std::endl;
                }
            }
        } else {
            std::cout << "Unknown command." << std::endl;
        }
    }
}

void signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received. Shutting down..." << std::endl;
    g_shutdown = true;
}

int main(int argc, char* argv[]) {
    int tcpPort = DEFAULT_TCP_PORT;
    int udpPort = DEFAULT_UDP_PORT;
    int httpPort = DEFAULT_HTTP_PORT;
    bool enableTCP = true;
    bool enableUDP = true;
    
    const struct option long_options[] = {
        {"tcpport",      required_argument, 0, 't'},
        {"udpport",      required_argument, 0, 'u'},
        {"httpport",     required_argument, 0, 'h'},
        {"disable-tcp",  no_argument,       0, 'x'},
        {"disable-udp",  no_argument,       0, 'y'},
        {"help",         no_argument,       0, '?'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "t:u:h:xy?", long_options, &option_index)) != -1) {
        switch(opt) {
            case 't':
                tcpPort = std::atoi(optarg);
                break;
            case 'u':
                udpPort = std::atoi(optarg);
                break;
            case 'h':
                httpPort = std::atoi(optarg);
                break;
            case 'x':
                enableTCP = false;
                break;
            case 'y':
                enableUDP = false;
                break;
            case '?':
            default:
                std::cout << "Usage: " << argv[0] << " [--tcpport <port>] [--udpport <port>] [--httpport <port>] [--disable-tcp] [--disable-udp]" << std::endl;
                return 0;
        }
    }

    signal(SIGINT, signalHandler);

    std::cout << "Server Controller starting..." << std::endl;
    std::cout << "TCP Port: " << tcpPort << " ( " << (enableTCP ? "enabled" : "disabled") << " )" << std::endl;
    std::cout << "UDP Port: " << udpPort << " ( " << (enableUDP ? "enabled" : "disabled") << " )" << std::endl;
    std::cout << "HTTP Port: " << httpPort << std::endl;

    std::vector<std::thread> threads;
    
    if (enableTCP) {
        threads.push_back(std::thread(tcpReceiver, tcpPort));
    }
    if (enableUDP) {
        threads.push_back(std::thread(udpReceiver, udpPort));
    }
    threads.push_back(std::thread(httpServer, httpPort));
    
    runtimeControlLoop();

    for(auto &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    std::cout << "Server Controller shutting down." << std::endl;
    return 0;
}