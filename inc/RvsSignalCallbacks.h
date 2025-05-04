#ifndef SIGNAL_CALLBACKS
#define SIGNAL_CALLBACKS

#include "RvsSignalEmitter.h"

void cb_set_brightness(CustomSignalEmitter *emitter, gpointer user_data);
void cb_set_saturation(CustomSignalEmitter *emitter, gpointer user_data);
void cb_set_contrast(CustomSignalEmitter *emitter, gpointer user_data);
void cb_set_pipeline_state(CustomSignalEmitter *emitter, gpointer user_data);

#endif /* SIGNAL_CALLBACKS */
