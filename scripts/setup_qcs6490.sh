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
    [ -d "${RVS_ROOT_DIR}/out" ]                                            && {
        mkdir "${RVS_ROOT_DIR}/out"
    }

    [ -d "${RVS_ROOT_DIR}/build" ]                                          && {
        mkdir "${RVS_ROOT_DIR}/build"
    }

    (
        cd "${PROJECT_BINARY_DIR}/build"
        cmake "${PROJECT_BINARY_DIR}/src"
    )
}

echo -e "\n####################################################################`
        `############"
echo "rvs-config-testapp-qcs6490()"
echo "    Used to configure the cmake build environment."

echo -e "\n####################################################################`
        `############"
echo "rvs-build-testapp-qcs6490()"
echo "    Used to build the usb-cam-to-web testapp."

