#!/bin/bash

function rvs-config-testapp-qcs6490() {

    # Sanity
    [ -z "${RVS_ROOT_DIR}" ]                                                && {
        echo -e "Environment varaible \"RVS_ROOT_DIR\" not set properly.\n`
                `Source the setenv.sh in the root of the project, while in the `
                `same directory as it is."
        return -1
    }

    # Create needed build and out directories
    [ ! -d "${RVS_ROOT_DIR}/build" ]                                        && {
        mkdir "${RVS_ROOT_DIR}/build"
    }

    # Configure the build
    (
        local RVS_SYSROOTS_DIR="${TOOLCHAIN_DIR}/tmp/sysroots/`                \
                `qcs6490-rb3gen2-vision-kit"
        cd "${RVS_ROOT_DIR}/build"                                          && \
            cmake                                                              \
                -DCMAKE_TOOLCHAIN_FILE=${OE_CMAKE_TOOLCHAIN_FILE}              \
                -DCMAKE_SYSROOT=${RVS_SYSROOTS_DIR}                            \
                -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON                               \
                -DCMAKE_BUILD_TYPE=Debug                                       \
                "${RVS_ROOT_DIR}"
    )
}

function rvs-build-testapp-qcs6490() {

    # Sanity
    [ -z "${RVS_ROOT_DIR}" ]                                                && {
        echo -e "Environment varaible \"RVS_ROOT_DIR\" not set properly.\n`
                `Source the setenv.sh in the root of the project, while in the `
                `same directory as it is."
        return -1
    }

    # Build and install
    (
        cd "${RVS_ROOT_DIR}/build"                                          && \
            cmake --build "${RVS_ROOT_DIR}/build"                           && \
            cmake --install "${RVS_ROOT_DIR}/build" -v
    )
}

echo -e "\n####################################################################`
        `############"
echo "rvs-config-testapp-qcs6490()"
echo -e "    Used to configure the cmake build environment.\n"

echo "rvs-build-testapp-qcs6490()"
echo "    Used to build the usb-cam-to-web testapp."

echo -e "####################################################################`
        `############"
