# FROM python:3.12-slim
# RUN apt-get update && apt-get install -y --no-install-recommends ffmpeg && rm -rf /var/lib/apt/lists/*
# WORKDIR /app
# COPY bridge.py .
# ENV PYTHONUNBUFFERED=1
# CMD ["python","bridge.py"]

#shell version
FROM alpine:3.19
RUN apk add --no-cache ffmpeg sqlite iproute2
COPY bridge.sh /bridge.sh
ENTRYPOINT ["/bridge.sh"]
