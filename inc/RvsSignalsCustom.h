#ifndef RVS_SIGNALS_CUSTOM
#define RVS_SIGNALS_CUSTOM

typedef enum {
    Invalid = -1,
    SetBrightness,
    SignalCount,
    SignalMax = 0x7fffffff
} RvsSignalCustom;

extern const char* signalName[SignalCount];

#endif /* RVS_SIGNALS_CUSTOM */
