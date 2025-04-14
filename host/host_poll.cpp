/* host_poller.cpp
    
    // Compiling 
    // sudo apt-get install libcurl4-openssl-dev
    // g++ -std=c++11 host_poll.cpp  -o host_poll -lcurl

   This application runs on the host device and polls a remote endpoint
   for instructions. It demonstrates extreme parametrization by using:
   - Preprocessor defines for default settings.
   - Command-line switches (via getopt_long) to override defaults.
   - A polling loop that makes HTTP GET requests using libcurl.
   - Debug output that can be enabled via a CLI switch.
 
   Default parameters:
     - Polling interval: 60 seconds (1 minute)
     - Remote endpoint URL: "https://example.com/api/instruction"
 
   Compile with (example):
     g++ host_poller.cpp -o host_poller -lcurl

   Run with:
     ./host_poller --polling 30 --endpoint "https://yourserver.com/poll" --debug
*/

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <getopt.h>
#include <curl/curl.h>

// Define default parameters
#define DEFAULT_POLLING_INTERVAL 60      // seconds
#define DEFAULT_ENDPOINT "https://example.com/api/instruction"

// Global flag for debug mode
bool g_debugEnabled = false;

// WriteCallback() is used by libcurl to capture the response data.
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::string* data = reinterpret_cast<std::string*>(userp);
    size_t totalSize = size * nmemb;
    data->append(reinterpret_cast<char*>(contents), totalSize);
    return totalSize;
}

// Prints the usage instructions.
void print_help(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options]\n"
              << "Options:\n"
              << "  -p, --polling INTERVAL   Set polling interval in seconds (default: " 
              << DEFAULT_POLLING_INTERVAL << ")\n"
              << "  -e, --endpoint URL       Set remote endpoint URL (default: " 
              << DEFAULT_ENDPOINT << ")\n"
              << "  -d, --debug              Enable debug messages\n"
              << "  -h, --help               Display this help and exit\n";
}

// poll_endpoint performs an HTTP GET on the given endpoint and returns the response.
std::string poll_endpoint(const std::string &endpoint) {
    if (g_debugEnabled) {
        std::cout << "[DEBUG] Polling endpoint: " << endpoint << std::endl;
    }
    
    CURL* curl = curl_easy_init();
    std::string readBuffer;
    if(curl) {
        // Set the target URL
        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
        // Use our callback function to store the response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        // Enable error messages for debugging
        if (g_debugEnabled) {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        }
        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK && g_debugEnabled) {
            std::cerr << "[DEBUG] curl_easy_perform() failed: " 
                      << curl_easy_strerror(res) << std::endl;
        }
        // Always cleanup
        curl_easy_cleanup(curl);
    } else if (g_debugEnabled) {
        std::cerr << "[DEBUG] Failed to initialize curl." << std::endl;
    }
    
    return readBuffer;
}

int main(int argc, char* argv[]) {
    int pollingInterval = DEFAULT_POLLING_INTERVAL;
    std::string endpoint = DEFAULT_ENDPOINT;

    // Define long options for command-line parsing
    const struct option long_options[] = {
        {"polling",  required_argument, 0, 'p'},
        {"endpoint", required_argument, 0, 'e'},
        {"debug",    no_argument,       0, 'd'},
        {"help",     no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    // Parse command-line arguments
    while ((opt = getopt_long(argc, argv, "p:e:dh", long_options, &option_index)) != -1) {
        switch(opt) {
            case 'p':
                pollingInterval = std::atoi(optarg);
                break;
            case 'e':
                endpoint = std::string(optarg);
                break;
            case 'd':
                g_debugEnabled = true;
                break;
            case 'h':
            default:
                print_help(argv[0]);
                return 0;
        }
    }

    // Ensure the polling interval is valid.
    if (pollingInterval <= 0) {
        std::cerr << "Error: Polling interval must be a positive integer." << std::endl;
        return 1;
    }

    // Output the initial configuration.
    std::cout << "Starting host poller with configuration:\n"
              << "  Polling Interval: " << pollingInterval << " seconds\n"
              << "  Endpoint: " << endpoint << "\n"
              << "  Debug: " << (g_debugEnabled ? "Enabled" : "Disabled") << std::endl;

    // Initialize libcurl global state (not strictly required if only one thread is used)
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Main polling loop.
    while (true) {
        std::string response = poll_endpoint(endpoint);
        if (!response.empty()) {
            std::cout << "Polled Response: " << response << std::endl;
        } else if (g_debugEnabled) {
            std::cout << "[DEBUG] Received empty response from endpoint." << std::endl;
        }
        // Sleep for the specified polling interval.
        std::this_thread::sleep_for(std::chrono::seconds(pollingInterval));
    }
    
    // Cleanup global curl state before exiting.
    curl_global_cleanup();
    
    return 0;
}
