#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>

#include "RvsTcpListener.h"
#include "RvsSignalsCustom.h"

static void *tcp_listener_thread(void *arg) {
    TcpListenerContext *ctx = (TcpListenerContext *)arg;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(9000) //needs to change with cli args
    };

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 3);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            continue;
        }

        char buffer[16] = {0};
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer)-1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            set_property(ctx, buffer);
        }
        close(client_fd);
    }
    return NULL;
}

void set_property(TcpListenerContext *ctx, char* buffer_ptr) {
    char command;
    buffer_ptr = strtok(buffer_ptr, ":");
    if (strlen(buffer_ptr) > 1) {
        g_printerr("Invalid Command \"%s\"\n", buffer_ptr);
        return;
    }
    command = buffer_ptr[0];
    switch(command) {
        case 'b': {
            buffer_ptr = strtok(NULL, ":");
            *(ctx->command_buffer) = strtol(buffer_ptr, NULL, 10);
            g_signal_emit_by_name(ctx->emitter, signalName[SetBrightness], ctx);
            break;
        }
        case 's': {
            buffer_ptr = strtok(NULL, ":");
            *(ctx->command_buffer) = strtol(buffer_ptr, NULL, 10);
            g_signal_emit_by_name(ctx->emitter, signalName[SetSaturation], ctx);
            break;
        }
        case 'c': {
            buffer_ptr = strtok(NULL, ":");
            *(ctx->command_buffer) = strtol(buffer_ptr, NULL, 10);
            g_signal_emit_by_name(ctx->emitter, signalName[SetContrast], ctx);
            break;
        }
        case 'p': {
            buffer_ptr = strtok(NULL, ":");
            *(ctx->command_buffer) = strtol(buffer_ptr, NULL, 10);
            g_signal_emit_by_name(ctx->emitter, signalName[SetState], ctx);
            break;
        }
        default: {
            g_printerr("Unsupported command: %c\n", command);
            break;
        }
    }
}

void start_tcp_listener(TcpListenerContext *ctx) {
    pthread_t thread;
    pthread_create(&thread, NULL, tcp_listener_thread, ctx);
    pthread_detach(thread);
}
