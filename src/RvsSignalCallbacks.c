#include "RvsSignalCallbacks.h"
#include "RvsTcpListener.h"

void cb_set_brightness(CustomSignalEmitter *emitter, gpointer user_data) {
    TcpListenerContext *ctx = (TcpListenerContext *)user_data;

    if (!ctx){
        g_printerr("In function %s: Missing context\n", __func__);
        return;
    }

    if (!ctx->media) {
        g_printerr("In function %s: Missing context media\n", __func__);
        return;
    }

    if (!ctx->command_buffer){
        g_printerr("In function %s: Missing context command buffer\n", __func__);
        return;
    }

    GstElement *pipeline = gst_rtsp_media_get_element(ctx->media);
    if (!pipeline) {
        g_printerr("In function %s: Could not get pipeline from media\n", __func__);
        return;
    }

    GstElement *src = gst_bin_get_by_name(GST_BIN(pipeline), "source");
    if (!src) {
        g_printerr("In function %s: 'source' element not found in pipeline\n", __func__);
        return;
    }

    int recieved_brightness_val = *(ctx->command_buffer);
    g_object_set(src, "brightness", recieved_brightness_val, NULL);
    g_print("Brightness changed via custom signal to %d\n", recieved_brightness_val);

    gst_object_unref(src);
}
