#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include <gst/gst.h>

#include "RvsSignalCallbacks.h"
#include "RvsTcpListener.h"
#include "RvsSignalEmitter.h"
#include "RvsSignalsCustom.h"

#ifdef TARGET_QCS6490_USE_HW_264_ENC

#define GST_LINE "v4l2src device=%s name=source ! "                                                \
                 "video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! "                  \
                 "videoconvert ! video/x-raw,format=NV12 ! "                                       \
                 "v4l2h264enc ! h264parse config-interval=1 ! "                                    \
                 "mpegtsmux ! udpsink host=%s port=%d"

#else

#define GST_LINE "v4l2src device=%s name=source ! "                                                \
                 "video/x-raw,format=YUY2,width=640,height=480,framerate=30/1 ! "                  \
                 "videoconvert ! video/x-raw,format=NV12 ! "                                       \
                 "x264enc ! h264parse config-interval=1 ! "                                        \
                 "mpegtsmux ! udpsink host=%s port=%d"

#endif // TARGET_QCS6490_USE_HW_264_ENC

void print_help(const char *prog) {
    g_print("\nUsage: %s -d <video_device> -i <destination_ip> -u <udp_port> -t <tcp_port>\n",
            prog
    );
    g_print("\nOptions:\n");
    g_print("  -d <device>        Video device path (e.g., /dev/video2)\n");
    g_print("  -i <ip>            Destination IP address for UDP stream\n");
    g_print("  -u <udp_port>      UDP port to stream to\n");
    g_print("  -t <tcp_port>      TCP port to listen on for control commands\n");
    g_print("  -h, --help         Show this help message\n\n");
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    const char *video_device = NULL;
    const char *dest_ip = NULL;
    uint16_t udp_port = 0;
    uint16_t tcp_port = 0;
    GstElement *pipeline;
    static int command_buffer[32] = {0};
    CustomSignalEmitter *emitter = NULL;
    TcpListenerContext *ctx = NULL;
    GstStateChangeReturn ret = GST_STATE_CHANGE_FAILURE;
    GMainLoop *loop = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "d:i:u:t:h")) != -1) {
        switch (opt) {
            case 'd': {
                video_device = optarg;
                break;
            }
            case 'i': {
                dest_ip = optarg;
                break;
            }
            case 'u': {
                udp_port = atoi(optarg);
                break;
            }
            case 't': {
                tcp_port = atoi(optarg);
                break;
            }
            case 'h': {
                print_help(argv[0]);
                return 0;
            }
            default: {
                print_help(argv[0]);
                return -1;
            }
        }
    }

    // Support for --help as well
    for (int i = 1; i < argc; ++i) {
        if (0 == strcmp(argv[i], "--help")) {
            print_help(argv[0]);
            return 0;
        }
    }

    if (!video_device || !dest_ip || udp_port <= 0 || tcp_port <= 0) {
        g_printerr("Error: Missing or invalid required arguments.\n");
        print_help(argv[0]);
        return -1;
    }

    char pipeline_description[1024];
    snprintf(pipeline_description, sizeof(pipeline_description),
            GST_LINE, video_device, dest_ip, udp_port);

    pipeline = gst_parse_launch(pipeline_description, NULL);
    if (!pipeline) {
        g_printerr("Failed to create pipeline.\n");
        return -1;
    }

    emitter = g_object_new(TYPE_CUSTOM_SIGNAL_EMITTER, NULL);
    ctx = g_new0(TcpListenerContext, 1);

    ctx->emitter = emitter;
    ctx->command_buffer = command_buffer;
    ctx->pipeline = pipeline;
    ctx->tcp_command_port = tcp_port;

    g_signal_connect(emitter, signalName[SetBrightness], G_CALLBACK(cb_set_brightness), ctx);
    g_signal_connect(emitter, signalName[SetSaturation], G_CALLBACK(cb_set_saturation), ctx);
    g_signal_connect(emitter, signalName[SetContrast], G_CALLBACK(cb_set_contrast), ctx);
    g_signal_connect(emitter, signalName[SetState], G_CALLBACK(cb_set_pipeline_state), ctx);
    start_tcp_listener(ctx);

    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to start pipeline.\n");
        gst_object_unref(pipeline);
        g_free(ctx);
        return -1;
    }

    g_print("Streaming to udp://%s:%d using device %s\n", dest_ip, udp_port, video_device);
    g_print("Listening for TCP commands on port %d\n", tcp_port);

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);
    if (NULL != ctx ) {
        g_free(ctx);
    }

    return 0;
}
