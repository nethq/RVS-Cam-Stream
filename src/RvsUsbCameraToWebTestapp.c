#include <gst/gst.h>

#include "RvsSignalCallbacks.h"
#include "RvsTcpListener.h"
#include "RvsSignalEmitter.h"
#include "RvsSignalsCustom.h"

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    if (argc < 3) {
        g_printerr("Usage: %s <DEST_IP> <VIDEO_DEVICE>\n", argv[0]);
        return -1;
    }

    const char *dest_ip = argv[1];
    const char *video_device = argv[2];

    // Create pipeline
    GstElement *pipeline = gst_pipeline_new("rvs-pipeline");

    GstElement *source = gst_element_factory_make("v4l2src", "source");
    GstElement *convert = gst_element_factory_make("videoconvert", "convert");
    GstElement *capsfilter = gst_element_factory_make("capsfilter", "caps");
    GstElement *encoder = gst_element_factory_make("v4l2h264enc", "encoder");
    GstElement *parser = gst_element_factory_make("h264parse", "parser");
    GstElement *rtppay = gst_element_factory_make("rtph264pay", "payloader");
    GstElement *udpsink = gst_element_factory_make("udpsink", "sink");

    if (!pipeline || !source || !convert || !capsfilter || !encoder || !parser || !rtppay || !udpsink) {
        g_printerr("Failed to create one or more GStreamer elements.\n");
        return -1;
    }

    g_object_set(source, "device", video_device, NULL);
    g_object_set(udpsink, "host", dest_ip, "port", 5000, NULL);
    g_object_set(rtppay, "pt", 96, NULL);

    // Configure caps
    GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                        "width", G_TYPE_INT, 640,
                                        "height", G_TYPE_INT, 480,
                                        "framerate", GST_TYPE_FRACTION, 30, 1,
                                        NULL);
    g_object_set(capsfilter, "caps", caps, NULL);
    gst_caps_unref(caps);

    // Assemble pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, convert, capsfilter, encoder, parser, rtppay, udpsink, NULL);
    if (!gst_element_link_many(source, convert, capsfilter, encoder, parser, rtppay, udpsink, NULL)) {
        g_printerr("Failed to link elements in pipeline.\n");
        return -1;
    }

    // Custom signal emitter
    CustomSignalEmitter *emitter = g_object_new(TYPE_CUSTOM_SIGNAL_EMITTER, NULL);

    // Command buffer shared with listener
    static int command_buffer[16] = {0};

    // TCP listener context
    TcpListenerContext *ctx = g_new0(TcpListenerContext, 1);
    ctx->emitter = emitter;
    ctx->command_buffer = command_buffer;
    ctx->source = source;  // Register source here

    // Hook brightness signal
    g_signal_connect(emitter, signalName[SetBrightness], G_CALLBACK(cb_set_brightness), ctx);

    // Start TCP listener on port 9000
    start_tcp_listener(ctx, 9000);

    // Set pipeline to PLAYING
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to start pipeline.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    g_print("Streaming to udp://%s:5000 using %s\n", dest_ip, video_device);

    // Run main loop
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    // Cleanup
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);
    g_free(ctx);

    return 0;
}
