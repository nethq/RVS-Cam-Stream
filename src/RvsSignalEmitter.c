#include "RvsSignalEmitter.h"
#include "RvsSignalsCustom.h"

G_DEFINE_TYPE(CustomSignalEmitter, custom_signal_emitter, G_TYPE_OBJECT)

static guint custom_signals[SignalCount] = { 0 };

void custom_signal_emitter_class_init(CustomSignalEmitterClass *klass) {
    custom_signals[SetBrightness] = g_signal_new(
        signalName[SetBrightness],
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_FIRST,
        0, NULL, NULL, NULL,
        G_TYPE_NONE, 0
    );

    custom_signals[SetSaturation] = g_signal_new(
        signalName[SetSaturation],
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_FIRST,
        0, NULL, NULL, NULL,
        G_TYPE_NONE, 0
    );

    custom_signals[SetContrast] = g_signal_new(
        signalName[SetContrast],
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_FIRST,
        0, NULL, NULL, NULL,
        G_TYPE_NONE, 0
    );

    custom_signals[SetState] = g_signal_new(
        signalName[SetState],
        G_TYPE_FROM_CLASS(klass),
        G_SIGNAL_RUN_FIRST,
        0, NULL, NULL, NULL,
        G_TYPE_NONE, 0
    );
}

void custom_signal_emitter_init(CustomSignalEmitter *self) {}
