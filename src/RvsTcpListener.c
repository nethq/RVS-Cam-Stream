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
        .sin_port = htons(9000)
    };

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 3);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        char buffer[16] = {0};
        char* buffer_ptr = 0;
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer)-1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            buffer_ptr = strtok(buffer, ":");
            // TODO: Insert switch case for callbacks
            buffer_ptr = strtok(NULL, ":");
            *(ctx->command_buffer) = atoi(buffer_ptr);
            // TODO: Change signalName array to not be statically fixed
            g_signal_emit_by_name(ctx->emitter, signalName[SetBrightness], ctx);
        }
        close(client_fd);
    }
    return NULL;
}

void start_tcp_listener(TcpListenerContext *ctx) {
    pthread_t thread;
    pthread_create(&thread, NULL, tcp_listener_thread, ctx);
    pthread_detach(thread);
}
