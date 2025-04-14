#!/usr/bin/env python3
"""
server_controller.py

This Python script acts as a central controller/receiver on the server side.
It does the following:
  - Listens for TCP connections on a configurable port. Hosts send commands
    (in the form "<stream_id> <command>") to register or change their stream state.
  - Listens for UDP data on a configurable port. UDP messages are assumed to start with
    a stream identifier followed by a space and payload.
  - Runs an HTTP server (using Flask) on a configurable port so that hosts can poll their
    own endpoint (e.g. GET /<stream_id>) to receive instructions in JSON format.
  - Provides a runtime control loop allowing an operator to list streams and toggle a stream’s viewing state.
  
Usage:
  python3 server_controller.py [--tcpport <port>] [--udpport <port>] [--httpport <port>]
                                [--disable-tcp] [--disable-udp]

Default ports:
  TCP: 9000
  UDP: 8000
  HTTP: 8080

Dependencies:
  - Flask (install via pip: pip3 install flask)
"""

import argparse
import threading
import socket
import sys
import time
import json

from flask import Flask, jsonify, request

DEFAULT_TCP_PORT = 9000
DEFAULT_UDP_PORT = 8000
DEFAULT_HTTP_PORT = 8080

shutdown_event = threading.Event()

# Global dictionary to hold stream states.
# Each stream is represented by a dictionary with keys: 'active' and 'viewing'
# Example: streams["stream1"] = {'active': True, 'viewing': False}
streams = {}
streams_lock = threading.Lock()

def tcp_receiver(tcp_port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    try:
        s.bind(("", tcp_port))
    except Exception as e:
        print("[TCP] Bind failed:", e)
        return

    s.listen(10)
    print(f"[TCP] Listening on port {tcp_port} ...")
    s.settimeout(1.0)
    
    while not shutdown_event.is_set():
        try:
            conn, addr = s.accept()
        except socket.timeout:
            continue
        except Exception as e:
            print("[TCP] Accept error:", e)
            continue

        with conn:
            try:
                # Read up to 1024 bytes; assume the message is a simple string
                data = conn.recv(1024).decode("utf-8")
                if not data:
                    continue
                # Expecting "stream_id command" (e.g., "cam123 START")
                parts = data.strip().split()
                if len(parts) < 2:
                    print("[TCP] Received malformed command:", data)
                    continue
                stream_id, command = parts[0], parts[1]
                with streams_lock:
                    if stream_id not in streams:
                        # Register new stream with default state
                        streams[stream_id] = {'active': False, 'viewing': False}
                        print(f"[TCP] New stream registered: {stream_id}")
                    if command.upper() == "START":
                        streams[stream_id]['active'] = True
                        print(f"[TCP] Stream {stream_id} set to ACTIVE.")
                    elif command.upper() == "STOP":
                        streams[stream_id]['active'] = False
                        print(f"[TCP] Stream {stream_id} set to INACTIVE.")
                    else:
                        print(f"[TCP] Unknown command for stream {stream_id}: {command}")
            except Exception as e:
                print("[TCP] Error processing connection:", e)
    s.close()
    print("[TCP] Shutting down TCP receiver.")

def udp_receiver(udp_port):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.bind(("", udp_port))
    except Exception as e:
        print("[UDP] Bind failed:", e)
        return
    s.settimeout(0.5)
    print(f"[UDP] Listening on port {udp_port} ...")
    while not shutdown_event.is_set():
        try:
            data, addr = s.recvfrom(2048)
        except socket.timeout:
            continue
        except Exception as e:
            print("[UDP] recvfrom error:", e)
            continue
        try:
            msg = data.decode("utf-8").strip()
            parts = msg.split(maxsplit=1)
            if not parts:
                continue
            stream_id = parts[0]
            payload = parts[1] if len(parts) > 1 else ""
            with streams_lock:
                if stream_id not in streams:
                    streams[stream_id] = {'active': False, 'viewing': False}
                    print(f"[UDP] New stream registered: {stream_id}")
            print(f"[UDP] Received data for stream {stream_id}: {payload}")
        except Exception as e:
            print("[UDP] Error processing message:", e)
    s.close()
    print("[UDP] Shutting down UDP receiver.")

app = Flask(__name__)

@app.route('/<stream_id>', methods=['GET'])
def get_instruction(stream_id):
    with streams_lock:
        if stream_id in streams:
            # If the stream is both active and being viewed, instruct "KEEP"; otherwise "STOP"
            if streams[stream_id]['active'] and streams[stream_id]['viewing']:
                instruction = {"command": "KEEP"}
            else:
                instruction = {"command": "STOP"}
        else:
            instruction = {"command": "UNKNOWN"}
    return jsonify(instruction)

def run_http_server(http_port):
    print(f"[HTTP] Server listening on port {http_port} ...")
    # Disable reloader and debug mode to avoid extra threads.
    app.run(host="0.0.0.0", port=http_port, threaded=True, use_reloader=False)

def runtime_control_loop():
    print("Runtime Control: Type 'list' to view streams, 'toggle <stream_id>' to toggle viewing state, or 'quit' to exit.")
    while not shutdown_event.is_set():
        try:
            cmd = input("> ").strip()
        except (EOFError, KeyboardInterrupt):
            shutdown_event.set()
            break

        if cmd.lower() == "quit":
            shutdown_event.set()
            break
        elif cmd.lower() == "list":
            with streams_lock:
                if streams:
                    print("Current streams:")
                    for sid, state in streams.items():
                        print(f"  [{sid}] Active={state['active']}, Viewing={state['viewing']}")
                else:
                    print("No streams registered yet.")
        elif cmd.startswith("toggle"):
            parts = cmd.split()
            if len(parts) < 2:
                print("Usage: toggle <stream_id>")
                continue
            stream_id = parts[1]
            with streams_lock:
                if stream_id in streams:
                    streams[stream_id]['viewing'] = not streams[stream_id]['viewing']
                    print(f"Stream [{stream_id}] viewing state set to {streams[stream_id]['viewing']}")
                else:
                    print(f"Stream [{stream_id}] not found.")
        else:
            print("Unknown command.")

def main():
    parser = argparse.ArgumentParser(description="Server Controller and Visualizer")
    parser.add_argument("--tcpport", type=int, default=DEFAULT_TCP_PORT,
                        help=f"TCP listening port (default: {DEFAULT_TCP_PORT})")
    parser.add_argument("--udpport", type=int, default=DEFAULT_UDP_PORT,
                        help=f"UDP listening port (default: {DEFAULT_UDP_PORT})")
    parser.add_argument("--httpport", type=int, default=DEFAULT_HTTP_PORT,
                        help=f"HTTP server port (default: {DEFAULT_HTTP_PORT})")
    parser.add_argument("--disable-tcp", action="store_true", help="Disable TCP listener")
    parser.add_argument("--disable-udp", action="store_true", help="Disable UDP listener")
    args = parser.parse_args()
    
    threads = []
    
    if not args.disable_tcp:
        tcp_thread = threading.Thread(target=tcp_receiver, args=(args.tcpport,))
        tcp_thread.start()
        threads.append(tcp_thread)
    
    if not args.disable_udp:
        udp_thread = threading.Thread(target=udp_receiver, args=(args.udpport,))
        udp_thread.start()
        threads.append(udp_thread)
    
    #async http server
    http_thread = threading.Thread(target=run_http_server, args=(args.httpport,))
    http_thread.start()
    threads.append(http_thread)
    
    try:
        runtime_control_loop()
    except KeyboardInterrupt:
        shutdown_event.set()
    
    for t in threads:
        t.join()
    
    print("Server Controller shutting down.")

if __name__ == "__main__":
    main()
