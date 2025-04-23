#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

#include "RvsSignalCallbacks.h"

#define GST_LINE "videotestsrc name=source ! video/x-raw,framerate=30/1,width=640,height=480 !     \
        videoconvert ! x264enc tune=zerolatency speed-preset=ultrafast bitrate=2048 !               \
        rtph264pay name=pay0 pt=96 config-interval=1"

int main(int argc, char *argv[]) {

    int test_in = 1;
    gpointer srcconfig = &test_in;

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    /* Create a custom signal emitter object and bind custom signal to a custom callback */
    CustomSignalEmitter *emitter = g_object_new(TYPE_CUSTOM_SIGNAL_EMITTER, NULL);
    g_signal_connect(emitter, "my-custom-signal", G_CALLBACK(on_my_custom_signal), NULL);

    /*
     * TODO: THE FOLLOWING CALLS EMIT THE SIGNAL. MOVE TO A CASE THAT RECEIVES TCP COMMANDS FROM SERVER
     * emit the custom signal in the main loop !!!
     *
     * CustomSignalEmitter *emitter = (CustomSignalEmitter *)userdata;  // Passed via user_data
     * g_signal_emit_by_name(emitter, "my-custom-signal");
     */

    /* Create the RTSP server */
    GstRTSPServer *server = gst_rtsp_server_new();

    if (NULL == argv[1] || NULL == argv[2]) {
        g_print("Provide argv[1] = ip; argv[2] = port\n");
        return -1;
    }

    /* Set the ip and port to bind the RTSP server */
    gst_rtsp_server_set_address (server, argv[1]);       /* Specify the IP address */
    gst_rtsp_server_set_service (server, argv[2]);       /* Specify the port */

    /* Get the mount points to attach media factory */
    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(server);

    /* Create a media factory for the stream */
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();

    /* used to demonstrate callback connection to change the pipeline on signal media-configure */
    g_signal_connect (factory, "media-configure", G_CALLBACK(media_configure), srcconfig);

    /*
     * TODO: Find a place for this:
     * g_signal_connect (factory, "media-configure", G_CALLBACK(media_configure), emitter);
     */

    /* Set the launch string for the media pipeline (video source, encoding, etc) */
    gst_rtsp_media_factory_set_launch(factory, GST_LINE);

    /* Attach the factory to a mount point */
    gst_rtsp_mount_points_add_factory(mounts, "/cam", factory);

    /* Attach the server to the default main context (using glib's main loop) */
    gst_rtsp_server_attach(server, NULL);

    /* Print server details */
    g_print("RTSP server is live at rtsp://%s:%s/cam\n", argv[1], argv[2]);

    /* Run the GLib main loop to keep the server alive */
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    return 0;
}
