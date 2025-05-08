FROM alpine:3.19
RUN apk add --no-cache ffmpeg sqlite iproute2
COPY bridge.sh /bridge.sh
ENTRYPOINT ["/bridge.sh"]
