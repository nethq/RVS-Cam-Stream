#ifndef RVS_TCP_LISTENER_H
#define RVS_TCP_LISTENER_H

#include <gst/gst.h>
#include <stdint.h>

#include "RvsSignalEmitter.h"

typedef struct {
    int *command_buffer;
    uint16_t tcp_command_port;
    CustomSignalEmitter *emitter;
    GstElement *pipeline;
} TcpListenerContext;


void start_tcp_listener(TcpListenerContext *ctx);

#endif /* RVS_TCP_LISTENER_H */
