export RVS_ROOT_DIR=$(pwd)

for SCRIPT in ${RVS_ROOT_DIR}/scripts/*.sh; do
    [ -e "${SCRIPT}" ]                                                      && {
        source "${SCRIPT}"
    }
done
