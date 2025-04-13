#include <iostream>
#include <gst/gst.h>

int main(int argc, char** argv) {

    /* TODO:
     * Initialize the gstreamer library.
     * Testing gst library linkage. Execute on device
     */

    GError* check_error;
    gst_init (&argc, &argv);
    gst_init_check (&argc, &argv, &check_error);

    std::cout << "I am ready!" << std::endl;
    return 0;
}
