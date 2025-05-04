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
}

// Other callbacks might be added in the same manner. After adding the callback function,
// it's a signal name must be associated with it in RvsSignalsCustom.c, the signal to be stored in
// RvsSignalEmitter.c and RvsSignalsCustom.h
// After this, the callback must be registered to a signal in the main testapp:
// g_signal_connect(), as well as in RvsSignalsCustom.c.
//
// The signal may be emitted in the tcp listener, with appropriate data.

void cb_set_saturation(CustomSignalEmitter *emitter, gpointer user_data) {
    TcpListenerContext *ctx = (TcpListenerContext *)user_data;

    if (!ctx || !ctx->command_buffer || !ctx->pipeline) {
        g_printerr("In function %s: Invalid context\n", __func__);
        return;
    }

    int saturation_val = *(ctx->command_buffer);
    g_print("Changing saturation to %d\n", saturation_val);

    // Lookup v4l2src by hardset name in the pipeline "source"
    GstElement *source = gst_bin_get_by_name(GST_BIN(ctx->pipeline), "source");
    if (!source) {
        g_printerr("v4l2src not found in pipeline\n");
        return;
    }

    // Apply saturation change
    g_object_set(source, "saturation", saturation_val, NULL);

    // Confirm value
    gint val = 0;
    g_object_get(source, "saturation", &val, NULL);
    g_print("Saturation after change: %d\n", val);

    gst_object_unref(source);  // Important to release the ref
}

void cb_set_contrast(CustomSignalEmitter *emitter, gpointer user_data) {
    TcpListenerContext *ctx = (TcpListenerContext *)user_data;

    if (!ctx || !ctx->command_buffer || !ctx->pipeline) {
        g_printerr("In function %s: Invalid context\n", __func__);
        return;
    }

    int contrast_val = *(ctx->command_buffer);
    g_print("Changing contrast to %d\n", contrast_val);

    // Lookup v4l2src by hardset name in the pipeline "source"
    GstElement *source = gst_bin_get_by_name(GST_BIN(ctx->pipeline), "source");
    if (!source) {
        g_printerr("v4l2src not found in pipeline\n");
        return;
    }

    // Apply contrast change
    g_object_set(source, "contrast", contrast_val, NULL);

    // Confirm value
    gint val = 0;
    g_object_get(source, "contrast", &val, NULL);
    g_print("Contrast after change: %d\n", val);

    gst_object_unref(source);  // Important to release the ref
}

// Pause - 0
// Play - 1
void cb_set_pipeline_state(CustomSignalEmitter *emitter, gpointer user_data) {
    TcpListenerContext *ctx = (TcpListenerContext *)user_data;

    if (!ctx || !ctx->command_buffer || !ctx->pipeline) {
        g_printerr("In function %s: Invalid context\n", __func__);
        return;
    }

    int set_state = *(ctx->command_buffer);
    GstState get_state;
    GstStateChangeReturn ret;
    g_print("Changing state to %d\n", set_state);

    switch (set_state) {
        case 0: {
            ret = gst_element_set_state(ctx->pipeline, GST_STATE_PAUSED);
            if (ret == GST_STATE_CHANGE_FAILURE) {
                g_printerr("Failed to pause pipeline\n");
            }
            ret = gst_element_get_state(ctx->pipeline, &get_state, NULL, GST_CLOCK_TIME_NONE);
            g_print("state: %s\n", gst_element_state_get_name(get_state));
        }
        case 1: {
            ret = gst_element_set_state(ctx->pipeline, GST_STATE_PLAYING);
            if (ret == GST_STATE_CHANGE_FAILURE) {
                g_printerr("Failed to resume pipeline\n");
            }
            ret = gst_element_get_state(ctx->pipeline, &get_state, NULL, GST_CLOCK_TIME_NONE);
            g_print("state: %s\n", gst_element_state_get_name(get_state));
        }
        default: {
            g_printerr("Not supported state\n");
        }
    }
}
