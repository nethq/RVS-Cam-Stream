#include "RvsSignalCallbacks.h"
#include "RvsTcpListener.h"

void on_my_custom_signal(CustomSignalEmitter *emitter, gpointer user_data) {
    TcpListenerContext *ctx = (TcpListenerContext *)user_data;

    if (!ctx || !ctx->media || !ctx->command_buffer) {
        g_printerr("Signal callback: Missing context data\n");
        return;
    }

    GstElement *pipeline = gst_rtsp_media_get_element(ctx->media);
    if (!pipeline) {
        g_printerr("Signal callback: Could not get pipeline from media\n");
        return;
    }

    GstElement *src = gst_bin_get_by_name(GST_BIN(pipeline), "source");
    if (!src) {
        g_printerr("Signal callback: 'source' element not found in pipeline\n");
        return;
    }

    int pattern = *(ctx->command_buffer);
    g_object_set(src, "pattern", pattern, NULL);
    g_print("Pattern changed via custom signal to %d\n", pattern);

    gst_object_unref(src);
}
