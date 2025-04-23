#include "RvsSignalEmitter.h"

G_DEFINE_TYPE(CustomSignalEmitter, custom_signal_emitter, G_TYPE_OBJECT)

static guint custom_signals[N_SIGNALS] = { 0 };

void custom_signal_emitter_class_init(CustomSignalEmitterClass *klass) {
    custom_signals[SIGNAL_MY_CUSTOM] = g_signal_new(
        "my-custom-signal",
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_FIRST,
        0,
        NULL, NULL,
        NULL,
        G_TYPE_NONE, // return type
        0            // no parameters
    );
}

void custom_signal_emitter_init(CustomSignalEmitter *self) {
    // Nothing to init for now
}
