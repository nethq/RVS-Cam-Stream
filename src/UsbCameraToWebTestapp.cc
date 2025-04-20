#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

int main(int argc, char *argv[]) {

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the RTSP server */
    GstRTSPServer *server = gst_rtsp_server_new();

    /* Set the ip and port to bind the RTSP server */
    gst_rtsp_server_set_address(server, "192.168.1.100");  /* Specify the IP address */
    gst_rtsp_server_set_service(server, "8554");            /* Specify the port */

    /* Get the mount points to attach media factory */
    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(server);

    /* Create a media factory for the stream */
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();

    /* Set the launch string for the media pipeline (video source, encoding, etc.) */
    gst_rtsp_media_factory_set_launch(factory,
        "( v4l2src device=/dev/video0 ! "
        "video/x-raw,framerate=30/1,width=640,height=480 ! "
        "videoconvert ! x264enc tune=zerolatency speed-preset=ultrafast "
        "bitrate=500 ! rtph264pay name=pay0 pt=96 config-interval=1 )");

    /* Attach the factory to a mount point */
    gst_rtsp_mount_points_add_factory(mounts, "/cam", factory);

    /* Unreference the mount points object after usage */
    g_object_unref(mounts);

    /* Attach the server to the default main context (using glib's main loop) */
    gst_rtsp_server_attach(server, NULL);

    /* Print server details. TODO: Make it custom. its 0130h here... */
    g_print("RTSP server is live at rtsp://192.168.1.100:8554/cam\n");

    /* Run the GLib main loop to keep the server alive */
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    return 0;
}
