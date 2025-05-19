export RVS_ROOT_DIR=$(pwd)
export RVS_NUM_CORES_MINUS_ONE=$(($(nproc --all) - 1))
export RVS_SERVER_IP="SERVER-IP-NEEDS-TO-BE-SET-BY-USER" # This is the ip of the host-server. Change accordingly. Check with ip -c a
export RVS_DEVICE_IP="169.254.0.105" # This is the ip of the device.
export RVS_SERVER_TOKEN="register123"
export RVS_IDENTITY_NAME="cam3"
export RVS_CONTROL_PORT_1="9000" #CONTROL PORT 1 - for python instance
export RVS_CONTROL_PORT_2="6001" #CONTROL PORT 2 - for host c app
export RVS_CAM_DEVICE="/dev/video2"
export RVS_BINARY="rvs-cam-to-web-testapp"

for SCRIPT in ${RVS_ROOT_DIR}/scripts/*.sh; do
    [ -e "${SCRIPT}" ]                                                      && {
        source "${SCRIPT}"
    }
done
