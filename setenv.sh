export RVS_ROOT_DIR=$(pwd)
export RVS_NUM_CORES_MINUS_ONE=$(($(nproc --all) - 1))
export RVS_SERVER_IP="127.0.0.1"
export RVS_SERVER_TOKEN="register123"
export RVS_IDENTITY_NAME="cam3"
export RVS_CONTROL_PORT_1="6000" #CONTROL PORT 1 - for python instance
export RVS_CONTROL_PORT_2="6001" #CONTROL PORT 2 - for host c app
export RVS_CAM_DEVICE="/dev/video0"
export RVS_BINARY="${RVS_ROOT_DIR}/out/rvs-cam-to-web-testapp"

for SCRIPT in ${RVS_ROOT_DIR}/scripts/*.sh; do
    [ -e "${SCRIPT}" ]                                                      && {
        source "${SCRIPT}"
    }
done
