#include "RvsSignalCallbacks.h"

void on_my_custom_signal(CustomSignalEmitter *emitter, gpointer user_data) {
    g_print("Custom signal was emitted!\n");
}

void media_configure (GstRTSPMediaFactory* factory, GstRTSPMedia* media, gpointer userdata) {
    GstElement* pipeline     = gst_rtsp_media_get_element (media);
    GstElement* videotestsrc = gst_bin_get_by_name ((GstBin*)pipeline, "source");
    if (videotestsrc) {
        g_object_set (videotestsrc, "pattern", *((int *)userdata), NULL);
        gst_object_unref (videotestsrc);
    }
}
