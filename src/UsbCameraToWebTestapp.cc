#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    /* Create the RTSP server */
    GstRTSPServer *server = gst_rtsp_server_new();
    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(server);

    /* Create the media factory */
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();

    /* Set the GStreamer pipeline for the RTSP media factory */
    gst_rtsp_media_factory_set_launch(factory,
        "( v4l2src device=/dev/video0 ! "
        "video/x-raw,framerate=30/1,width=640,height=480 ! "
        "queue max-size-buffers=1 leaky=downstream ! "
        "videoconvert ! x264enc tune=zerolatency speed-preset=ultrafast "
        "bitrate=500 ! rtph264pay name=pay0 pt=96 config-interval=1 )");

    /* Set the endpoint path (e.g., rtsp://localhost:8554/cam) */
    gst_rtsp_mount_points_add_factory(mounts, "/cam", factory);
    g_object_unref(mounts);

    /* Attach the server to the default main context */
    gst_rtsp_server_attach(server, NULL);

    g_print("RTSP server is live at rtsp://127.0.0.1:8554/cam\n");

    /* Run the main loop */
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    return 0;
}
