#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

#include "RvsSignalCallbacks.h"
#include "RvsTcpListener.h"
#include "RvsSignalEmitter.h"
#include "RvsSignalsCustom.h"

// #define GST_LINE "v4l2src device=/dev/video2 name=source ! video/x-raw,framerate=30/1,width=640,height=480 ! " \
//                  "videoconvert ! v4l2h264enc ! " \
//                  "rtph264pay name=pay0 pt=96 config-interval=1"

#define GST_LINE "v4l2src device=/dev/video2 name=source ! queue ! video/x-raw,format=YUY2,framerate=30/1,width=640,height=480 ! " \
                 "videoconvert ! queue ! video/x-raw,format=NV12 ! " \
                 "v4l2h264enc ! queue ! " \
                 "rtph264pay name=pay0 pt=96 config-interval=1"


static TcpListenerContext *listener_ctx = NULL;

void media_configure(GstRTSPMediaFactory *factory, GstRTSPMedia *media, gpointer user_data) {
    if (listener_ctx) {
        listener_ctx->media = media;
    }
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    if (argc < 3) {
        g_print("Usage: %s <IP> <PORT>\n", argv[0]);
        return -1;
    }

    const char *ip = argv[1];
    const char *port = argv[2];

    // Custom signal emitter
    CustomSignalEmitter *emitter = g_object_new(TYPE_CUSTOM_SIGNAL_EMITTER, NULL);

    // Command buffer
    static char command_buffer[16] = {0};

    // Create RTSP server
    GstRTSPServer *server = gst_rtsp_server_new();
    gst_rtsp_server_set_address(server, ip);
    gst_rtsp_server_set_service(server, port);

    // Mount points and factory
    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(server);
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();

    // Create listener context
    listener_ctx = g_new0(TcpListenerContext, 1);
    listener_ctx->emitter = emitter;
    listener_ctx->command_buffer = &command_buffer;

    // Connect custom signal
    g_signal_connect(emitter, signalName[SetBrightness],
            G_CALLBACK(cb_set_brightness), listener_ctx);

    // Connect media-configure to capture media and set initial pattern
    g_signal_connect(factory, "media-configure", G_CALLBACK(media_configure), &command_buffer);

    // Set pipeline launch description
    gst_rtsp_media_factory_set_launch(factory, GST_LINE);
    gst_rtsp_mount_points_add_factory(mounts, "/cam", factory);

    // Attach server
    gst_rtsp_server_attach(server, NULL);
    g_print("/dev/video2 RTSP server is live at rtsp://%s:%s/cam\n", ip, port);

    // Start TCP listener on port 9000
    start_tcp_listener(listener_ctx, 9000);

    // Start main loop
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    return 0;
}
