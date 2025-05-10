function rvs-make-db() {
sqlite3 ${RVS_ROOT_DIR}/server/store.db <<-'EOF'
PRAGMA journal_mode = DELETE;
CREATE TABLE IF NOT EXISTS cams (
name  TEXT PRIMARY KEY,
udp   INT  UNIQUE,
ctrl1 INT,
ctrl2 INT,
ip    TEXT
);
EOF
}

function annihilate-server() {
    cd ${RVS_ROOT_DIR}/server/                                             && \
        docker compose down --rmi all                             
}

function rvs-setup-server() {
    [ ! -f ${RVS_ROOT_DIR}/server/store.db ]                               && {
        rvs-make-db                                                        && \
            cd ${RVS_ROOT_DIR}/server/                                     && \
            docker compose up -d --build                                    && \
            echo "DB Built, "                                              && \                                
            echo "Docker container started, at ${RVS_SERVER_IP}:4000"                                
    }                                                                      || {
        cd ${RVS_ROOT_DIR}/server/                                         && \
            docker compose up -d --build                                   && \
            echo "Docker container started, web-ui at ${RVS_SERVER_IP}:4000"                                
    }
}

echo -e "####################################################################`
    `############"
echo "    rvs-setup-server() -- Starts docker compose procedure for server build"
echo "    rvs-make-db() -- Creates server/store.db, and inits if doesn't exist."
echo -e "####################################################################`
    `############"
