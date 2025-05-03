#include <gst/gst.h>

#include "RvsSignalCallbacks.h"
#include "RvsTcpListener.h"

void cb_set_brightness(CustomSignalEmitter *emitter, gpointer user_data) {
    TcpListenerContext *ctx = (TcpListenerContext *)user_data;

    if (!ctx || !ctx->command_buffer || !ctx->pipeline) {
        g_printerr("In function %s: Invalid context\n", __func__);
        return;
    }

    int brightness_val = *(ctx->command_buffer);
    g_print("Changing brightness to %d\n", brightness_val);

    // Pause pipeline
    // Pausing the pipeline is not needed, but was left here for compatability reasons.
    GstStateChangeReturn ret = gst_element_set_state(ctx->pipeline, GST_STATE_PAUSED);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to pause pipeline before brightness change\n");
        return;
    }

    // Lookup v4l2src by hardset name in the pipeline "source"
    GstElement *source = gst_bin_get_by_name(GST_BIN(ctx->pipeline), "source");
    if (!source) {
        g_printerr("v4l2src not found in pipeline\n");
        return;
    }

    // Apply brightness change
    g_object_set(source, "brightness", brightness_val, NULL);

    // Confirm value
    gint val = 0;
    g_object_get(source, "brightness", &val, NULL);
    g_print("Brightness after change: %d\n", val);

    gst_object_unref(source);  // Important to release the ref

    // Resume pipeline
    ret = gst_element_set_state(ctx->pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to resume pipeline after brightness change\n");
    }
}

// Other callbacks might be added in the same manner. After adding the callback function,
// it's a signal name must be associated with it in RvsSignalsCustom.c, the signal to be stored in
// RvsSignalEmitter.c and RvsSignalsCustom.h
// After this, the callback must be registered to a signal in the main testapp:
// g_signal_connect(), as well as in RvsSignalsCustom.c.
//
// The signal may be emitted in the tcp listener, with appropriate data.
