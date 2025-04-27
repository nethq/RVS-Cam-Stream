#ifndef SIGNAL_CALLBACKS
#define SIGNAL_CALLBACKS

#include <gst/rtsp-server/rtsp-server.h>

#include "RvsSignalEmitter.h"

void cb_set_brightness(CustomSignalEmitter *emitter, gpointer user_data);

#endif /* SIGNAL_CALLBACKS */
