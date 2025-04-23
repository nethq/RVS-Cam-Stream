#ifndef SIGNAL_EMITTER
#define SIGNAL_EMITTER

#include <glib-object.h>

#define TYPE_CUSTOM_SIGNAL_EMITTER (custom_signal_emitter_get_type())

G_DECLARE_FINAL_TYPE(CustomSignalEmitter, custom_signal_emitter, CUSTOM, SIGNAL_EMITTER, GObject)

struct _CustomSignalEmitter {
    GObject parent_instance;
};

enum {
    SIGNAL_MY_CUSTOM,
    N_SIGNALS
};

#endif /* SIGNAL_EMITTER */
