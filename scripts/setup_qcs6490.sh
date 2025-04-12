#!/bin/bash

function rvs-config-testapp-qcs6490() {

    # Sanity
    [ -z "${RVS_ROOT_DIR}" ] && {
        echo -e "Environment varaible \"RVS_ROOT_DIR\" not set properly.\n`
                `Source the setenv.sh in the root of the project, while in the `
                `same directory as it is."
        return -1
    }

    # Create needed build and out directories
    [ ! -d "${RVS_ROOT_DIR}/build" ]                                       && {
        mkdir "${RVS_ROOT_DIR}/build"
    }

    (
        cd "${RVS_ROOT_DIR}/build"
        cmake "${RVS_ROOT_DIR}"
    )
}

function rvs-build-testapp-qcs6490() {

    # Sanity
    [ -z "${RVS_ROOT_DIR}" ] && {
        echo -e "Environment varaible \"RVS_ROOT_DIR\" not set properly.\n`
                `Source the setenv.sh in the root of the project, while in the `
                `same directory as it is."
        return -1
    }

    (
        cd "${RVS_ROOT_DIR}/build"                                          && \
                cmake --build "${RVS_ROOT_DIR}/build"                       && \
                cmake --install "${RVS_ROOT_DIR}/build"
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