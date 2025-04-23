#ifndef SIGNAL_CALLBACKS
#define SIGNAL_CALLBACKS

#include <gst/rtsp-server/rtsp-server.h>

#include "RvsSignalEmitter.h"

void on_my_custom_signal(CustomSignalEmitter *emitter, gpointer user_data);
void media_configure (GstRTSPMediaFactory* factory, GstRTSPMedia* media, gpointer userdata);

#endif /* SIGNAL_CALLBACKS */
