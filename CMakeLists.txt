cmake_minimum_required(VERSION 3.10)

project(RVSCamToWeb)

# # Find needed gstreamer includes and libs
# find_package(PkgConfig REQUIRED)
# pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0)
# pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0)
# pkg_search_module(gstreamer-rtsp-server REQUIRED IMPORTED_TARGET gstreamer-rtsp-server-1.0)
# pkg_search_module(glib2 REQUIRED IMPORTED_TARGET glib-2.0)

# # Add main testapp source
# add_executable(cam-to-web-testapp
#     src/UsbCameraToWebTestapp.cc
# )

# # Link main testapp to needed gstreamer libs (some of them may be redundant)
# target_link_libraries(cam-to-web-testapp
#     PkgConfig::gstreamer
#     PkgConfig::gstreamer-video
#     gstreamer-rtsp-server
#     glib2
# )

# # Install compiled objects to ${}
# install(TARGETS cam-to-web-testapp
#     DESTINATION ${CMAKE_SOURCE_DIR}/out
# )

# Find needed gstreamer includes and libs
find_package(PkgConfig REQUIRED)

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
)

# Add main testapp source
add_executable(cam-to-web-testapp
    src/UsbCameraToWebTestapp.cc
)

# Link main testapp to needed gstreamer libs (some of them may be redundant)
target_link_libraries(cam-to-web-testapp
    ${gstreamer_LIBRARIES}
    ${gstreamer-video_LIBRARIES}
    ${gstreamer-rtsp-server_LIBRARIES}
    ${glib2_LIBRARIES}
)

# Install compiled objects to the specified directory
install(TARGETS cam-to-web-testapp
    DESTINATION ${CMAKE_SOURCE_DIR}/out
)

