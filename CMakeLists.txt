cmake_minimum_required(VERSION 3.10)

project(RVSCamToWeb)

# Find needed gstreamer includes and libs
find_package(PkgConfig REQUIRED)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0)
pkg_search_module(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0)

# Add main testapp source
add_executable(cam-to-web-testapp
    src/UsbCameraToWebTestapp.cc
)

# Link main testapp to needed gstreamer libs (some of them may be redundant)
target_link_libraries(cam-to-web-testapp
    PkgConfig::gstreamer
    PkgConfig::gstreamer-sdp
    PkgConfig::gstreamer-app
    PkgConfig::gstreamer-video
)

# Install compiled objects to ${}
install(TARGETS cam-to-web-testapp
    DESTINATION ${CMAKE_SOURCE_DIR}/out
)
