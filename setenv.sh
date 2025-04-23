export RVS_ROOT_DIR=$(pwd)
export RVS_NUM_CORES_MINUS_ONE=$(($(nproc --all) - 1))

for SCRIPT in ${RVS_ROOT_DIR}/scripts/*.sh; do
    [ -e "${SCRIPT}" ]                                                      && {
        source "${SCRIPT}"
    }
done
