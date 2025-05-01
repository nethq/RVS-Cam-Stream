cmake_minimum_required(VERSION 3.10)

project(RVSCamToWeb)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# GStreamer and GLib
pkg_search_module(gstreamer REQUIRED gstreamer-1.0)
pkg_search_module(gstreamer-video REQUIRED gstreamer-video-1.0)
pkg_search_module(glib2 REQUIRED glib-2.0)

# Include directories
include_directories(
    ${gstreamer_INCLUDE_DIRS}
    ${gstreamer-video_INCLUDE_DIRS}
    ${glib2_INCLUDE_DIRS}
    inc/
)

link_directories(
    ${gstreamer_LIBRARY_DIRS}
    ${gstreamer-video_LIBRARY_DIRS}
    ${glib2_LIBRARY_DIRS}
)

# CUSTOM SIGNALS
add_library(rvs-custom-signals STATIC
    src/RvsSignalsCustom.c
)

# SIGNAL EMITTER
add_library(rvs-signal-emitter SHARED
    src/RvsSignalEmitter.c
)

target_link_libraries(rvs-signal-emitter
    ${gstreamer_LIBRARIES}
    ${glib2_LIBRARIES}
    rvs-custom-signals
)

# SIGNAL CALLBACKS
add_library(rvs-signal-callbacks SHARED
    src/RvsSignalCallbacks.c
)

target_link_libraries(rvs-signal-callbacks
    ${gstreamer_LIBRARIES}
    ${glib2_LIBRARIES}
    rvs-custom-signals
    rvs-signal-emitter
)

# TCP LISTENER
add_library(rvs-tcp-listener SHARED
    src/RvsTcpListener.c
)

target_link_libraries(rvs-tcp-listener
    ${glib2_LIBRARIES}
    Threads::Threads
    rvs-custom-signals
)

# TEST APPLICATION
add_executable(rvs-cam-to-web-testapp
    src/RvsUsbCameraToWebTestapp.c
)

target_link_libraries(rvs-cam-to-web-testapp
    ${gstreamer_LIBRARIES}
    ${gstreamer-video_LIBRARIES}
    ${glib2_LIBRARIES}
    Threads::Threads
    rvs-signal-emitter
    rvs-signal-callbacks
    rvs-tcp-listener
    rvs-custom-signals
)

# Install targets
install(TARGETS
    rvs-cam-to-web-testapp
    rvs-signal-emitter
    rvs-signal-callbacks
    rvs-tcp-listener
    DESTINATION ${CMAKE_SOURCE_DIR}/out
)
