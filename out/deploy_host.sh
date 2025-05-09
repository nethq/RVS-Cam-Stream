#!/usr/bin/env bash
set -euo pipefail

# Process:
# - Host device authenticates to <server>/register endpoint
# - Endpoint responds with udp port, server starts listening on udp port
# - C APP is started with allocated udp port from server, and ctrl2 port as tcp listener
# - PY listener is started on first ctrl port, listening for halt=1
# *** if killed with halt, through py listener, will clean and purge 
# *** all existing instances of rvs-cam-to-web-testapp

SERVER=${SERVER:-127.0.0.1} #server
TOKEN=${TOKEN:-register123} #registration token for auth with backend
NAME=${NAME:-cam3} 
CTRL1=${CTRL1:-6000} #-|
CTRL2=${CTRL2:-6001} #-|TCP ports, where instructions to control the pipeline will be sent
DEV=${DEV:-/dev/video0} # - video device to stream

# ensure we always clean up on exit or interrupt
cleanup() {
  echo "[$NAME] cleaning up…"
  kill "$STREAM_PID" 2>/dev/null || true
  pkill -f rvs-cam-to-web-testapp 2>/dev/null || true
  exit 0
}
trap cleanup INT TERM

# retry registration until the server is reachable and returns a udpPort
until resp=$(curl -sf -X POST "http://$SERVER:4000/register" \
    -H 'Content-Type: application/json' \
    -d "{\"key\":\"$TOKEN\",\"name\":\"$NAME\",\"ctrl1\":$CTRL1,\"ctrl2\":$CTRL2}"); do
  echo "[$NAME] registration failed, retrying in 5s…"
  sleep 5
done

UDP=$(printf '%s' "$resp" | grep -Po '"udpPort":\K\d+')
echo "[$NAME] udp $UDP allocated"

# start the streaming app
./rvs-cam-to-web-testapp -d "$DEV" -i "$SERVER" -u "$UDP" -t "$CTRL1" &
STREAM_PID=$!

# listen for control commands (including halt=1)
python3 - <<PY
import socket, os, signal, sys
srv, c1, c2, pid = "$SERVER", $CTRL1, $CTRL2, $STREAM_PID
s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(('', c1))
s.listen(1)
try:
    while True:
        conn, _ = s.accept()
        msg = conn.recv(64).strip()
        if msg == b'halt=1':
            print("[$NAME] halt=1 received, stopping stream")
            os.kill(pid, signal.SIGINT)
            break
        for port in (c1, c2):
            try:
                sock = socket.create_connection((srv, port), timeout=1)
                sock.sendall(msg)
                sock.close()
            except:
                pass
        conn.close()
finally:
    s.close()
    sys.exit(0)
PY

# final cleanup
cleanup
