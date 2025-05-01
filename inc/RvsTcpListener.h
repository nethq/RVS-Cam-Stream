#ifndef RVS_TCP_LISTENER_H
#define RVS_TCP_LISTENER_H

#include <gst/gst.h>

#include "RvsSignalEmitter.h"

typedef struct {
    CustomSignalEmitter *emitter;
    int *command_buffer;
    GstElement *source;  // Direct reference to source
} TcpListenerContext;


void start_tcp_listener(TcpListenerContext *ctx, int port);

#endif /* RVS_TCP_LISTENER_H */
