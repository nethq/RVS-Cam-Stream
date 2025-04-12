cmake_minimum_required(VERSION 3.10)
project(RVSCamToWeb)


add_executable(cam-to-web-testapp
    src/UsbCameraToWebTestapp.cc
)

install(TARGETS cam-to-web-testapp
    DESTINATION ${CMAKE_BINARY_DIR}/out
)