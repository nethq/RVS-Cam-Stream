#!/bin/bash

function rvs-config-and-build-testapp-amd64() {

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

    # Configure and build the build
    (
        cd "${RVS_ROOT_DIR}/build"                                          && \
            cmake                                                              \
                -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON                               \
                -DCMAKE_BUILD_TYPE=Debug                                       \
                "${RVS_ROOT_DIR}"                                           && \
            cmake --build "${RVS_ROOT_DIR}/build"                              \
                    -j ${RVS_NUM_CORES_MINUS_ONE}                           && \
            cmake --install "${RVS_ROOT_DIR}/build" -v
    )
}

echo -e "####################################################################`
        `############"
echo "rvs-config-and-build-testapp-amd64()"
echo "    Used to build the usb-cam-to-web testapp for target x84_64."
echo -e "####################################################################`
        `############"
