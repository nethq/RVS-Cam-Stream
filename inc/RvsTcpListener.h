#ifndef RVS_TCP_LISTENER_H
#define RVS_TCP_LISTENER_H

#include "RvsSignalEmitter.h"
#include <gst/rtsp-server/rtsp-server.h>

typedef struct {
    CustomSignalEmitter *emitter;
    GstRTSPMedia *media;
    int *command_buffer;
} TcpListenerContext;

void start_tcp_listener(TcpListenerContext *ctx, int port);

#endif
