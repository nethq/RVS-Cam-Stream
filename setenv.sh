# Make newline the only separator. Used to "sanitize" directories with whitespace
export IFS=$'\n'
export RVS_ROOT_DIR=$(pwd)

for SCRIPT in ${RVS_ROOT_DIR}/scripts/*.sh; do
    [ -e "${SCRIPT}" ]                                                      && {
        source "${SCRIPT}"
    }
done
