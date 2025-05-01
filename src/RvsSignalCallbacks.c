#include <gst/gst.h>

#include "RvsSignalCallbacks.h"
#include "RvsTcpListener.h"

void cb_set_brightness(CustomSignalEmitter *emitter, gpointer user_data) {
    TcpListenerContext *ctx = (TcpListenerContext *)user_data;

    if (!ctx || !ctx->source || !ctx->command_buffer) {
        g_printerr("In function %s: Invalid context\n", __func__);
        return;
    }

    int brightness_val = *(ctx->command_buffer);
    g_object_set(ctx->source, "brightness", brightness_val, NULL);
    g_print("Brightness changed via custom signal to %d\n", brightness_val);
}

