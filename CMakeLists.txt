cmake_minimum_required(VERSION 3.10)

project(RVSCamToWeb)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Find needed gstreamer includes and libs
find_package(PkgConfig REQUIRED)

# Find pthreads
find_package(Threads REQUIRED)

# GStreamer libraries
pkg_search_module(gstreamer REQUIRED gstreamer-1.0)
pkg_search_module(gstreamer-video REQUIRED gstreamer-video-1.0)
pkg_search_module(gstreamer-rtsp-server REQUIRED gstreamer-rtsp-server-1.0)
pkg_search_module(glib2 REQUIRED glib-2.0)

# Include directories for GStreamer and glib
include_directories(
    ${gstreamer_INCLUDE_DIRS}
    ${gstreamer-video_INCLUDE_DIRS}
    ${gstreamer-rtsp-server_INCLUDE_DIRS}
    ${glib2_INCLUDE_DIRS}
    inc/
)

link_directories(
    ${gstreamer_LIBRARY_DIRS}
    ${gstreamer-video_LIBRARY_DIRS}
    ${gstreamer-rtsp-server_LIBRARY_DIRS}
    ${glib2_LIBRARY_DIRS}
)

# SIGNAL EMITTER
# Add the library that enables emission of custom signals
add_library(rvs-signal-emitter SHARED
    src/RvsSignalEmitter.c)

target_link_libraries(rvs-signal-emitter
    ${gstreamer_LIBRARIES}
    ${glib2_LIBRARIES}
)

# SIGNAL CALLBACKS
# Add the library that contains the callback functions.
add_library(rvs-signal-callbacks SHARED
    src/RvsSignalCallbacks.c)

target_link_libraries(rvs-signal-callbacks
    ${gstreamer_LIBRARIES}
    ${gstreamer-rtsp-server_LIBRARIES}
    ${glib2_LIBRARIES}
)

# TCP LISTENER
# Add the library that contains the callback functions.
add_library(rvs-tcp-listener SHARED
    src/RvsTcpListener.c)

target_link_libraries(rvs-tcp-listener
    ${glib2_LIBRARIES}
    Threads::Threads
)

# TESTAPP
# Add main testapp source
add_executable(rvs-cam-to-web-testapp
    src/RvsUsbCameraToWebTestapp.c
)

# Link main testapp to needed gstreamer libs (some of them may be redundant)
target_link_libraries(rvs-cam-to-web-testapp
    ${gstreamer_LIBRARIES}
    ${gstreamer-rtsp-server_LIBRARIES}
    ${glib2_LIBRARIES}
    rvs-signal-emitter
    rvs-signal-callbacks
    rvs-tcp-listener
)

# Install compiled objects to the specified directory
install(TARGETS
    rvs-cam-to-web-testapp
    rvs-signal-emitter
    rvs-signal-callbacks
    rvs-tcp-listener
    DESTINATION ${CMAKE_SOURCE_DIR}/out
)
