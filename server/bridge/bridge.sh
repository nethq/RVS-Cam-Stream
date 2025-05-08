#!/usr/bin/env sh
set -eu
DB=/data/store.db
HOST_IP=$(ip route get 1 | awk '{for(i=1;i<=NF;i++) if($i=="src"){print $(i+1);exit}}')
while true; do
  sqlite3 "$DB" 'SELECT name,udp FROM cams' | while IFS='|' read n u; do
    [ -z "$n" ] && continue
    pgrep -f "udp://0.0.0.0:$u" >/dev/null 2>&1 && continue
    echo "[bridge] ingest udp://0.0.0.0:$u  ->  $n"
    echo "[bridge] view   http://$HOST_IP:8888/$n/index.m3u8"
    ffmpeg -nostats -loglevel warning -fflags +nobuffer \
      -i "udp://0.0.0.0:$u?listen=1" \
      -c copy -bsf:v h264_mp4toannexb -f flv "rtmp://localhost:1935/$n" &
  done
  sleep 1
done
