#ifndef RVS_SIGNAL_EMITTER
#define RVS_SIGNAL_EMITTER

#include <glib-object.h>

#define TYPE_CUSTOM_SIGNAL_EMITTER (custom_signal_emitter_get_type())

G_DECLARE_FINAL_TYPE(CustomSignalEmitter, custom_signal_emitter, CUSTOM, SIGNAL_EMITTER, GObject)

struct _CustomSignalEmitter {
    GObject parent_instance;
};

#endif /* RVS_SIGNAL_EMITTER */
