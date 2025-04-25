#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>

#include "RvsTcpListener.h"

static void *tcp_server_thread(void *arg) {
    TcpListenerContext *ctx = (TcpListenerContext *)arg;
    int server_fd, client_fd;
    struct sockaddr_in addr;
    int opt = 1;
    int addrlen = sizeof(addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9000);  // default port, override if needed

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 3);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&addr, (socklen_t*)&addrlen);
        if (client_fd < 0) continue;

        char buffer[16] = {0};
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // null-terminate to make it a valid C string
            int pattern = atoi(buffer); // safely parse integer
            *(ctx->command_buffer) = pattern;

            // Emit signal
            g_signal_emit_by_name(ctx->emitter, "my-custom-signal", ctx);
        }
        close(client_fd);
    }

    return NULL;
}

void start_tcp_listener(TcpListenerContext *ctx, int port) {
    pthread_t thread;
    pthread_create(&thread, NULL, tcp_server_thread, ctx);
    pthread_detach(thread);
}
