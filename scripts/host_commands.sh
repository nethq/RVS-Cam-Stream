#!/usr/bin/env bash

rvs-register-device() {
  echo "[${RVS_IDENTITY_NAME}] Attempting registration with ${RVS_SERVER_IP}"

  until resp=$(curl -sf -X POST "http://${RVS_SERVER_IP}:4000/register" \
    -H 'Content-Type: application/json' \
    -d "{\"key\":\"${RVS_SERVER_TOKEN}\",\"name\":\"${RVS_IDENTITY_NAME}\",\"ctrl1\":${RVS_CONTROL_PORT_1},\"ctrl2\":${RVS_CONTROL_PORT_2}}"
  ); do
    echo "[${RVS_IDENTITY_NAME}] registration failed, retrying in 5s…"
    sleep 5
  done

  export RVS_UDP_PORT=$(printf '%s' "${resp}" | grep -Po '"udpPort":\K\d+')
  echo "[${RVS_IDENTITY_NAME}] udp ${RVS_UDP_PORT} allocated"
}

# Used to set a custom ip to the device, in order to sucessfully send tcp commands from server
rvs-setup-device-ip() {
    local RVS_DEVICE_NW_IFACE="eth1"
    adb shell "ip addr add ${RVS_DEVICE_IP}/16 dev ${RVS_DEVICE_NW_IFACE}"                  && \
        adb shell "ip link set ${RVS_DEVICE_NW_IFACE} up"
}

rvs-embedded-start-stream() {
  if [[ -z "${RVS_UDP_PORT}" || "${RVS_UDP_PORT}" -eq 0 ]]; then
    echo "[ERR] UDP port not set. Run rvs-register-device first."
    return 1
  fi

  rvs-setup-device-ip

  echo "[${RVS_IDENTITY_NAME}] Starting stream using ${RVS_BINARY}"
  adb shell "${RVS_BINARY}" -d "${RVS_CAM_DEVICE}" -i "${RVS_SERVER_IP}" -u "${RVS_UDP_PORT}" -t "${RVS_CONTROL_PORT_1}" &
  export RVS_STREAM_PID=$!
}

rvs-local-start-stream(){
  if [[ -z "${RVS_UDP_PORT}" || "${RVS_UDP_PORT}" -eq 0 ]]; then
    echo "[ERR] UDP port not set. Run rvs-register-device first."
    return 1
  fi

  echo "[${RVS_IDENTITY_NAME}] Starting stream using ${RVS_BINARY}"
  echo "hello ${RVS_BINARY}" -d "${RVS_CAM_DEVICE}" -i "${RVS_SERVER_IP}" -u "${RVS_UDP_PORT}" -t "${RVS_CONTROL_PORT_1}"
  "${RVS_BINARY}" -d "${RVS_CAM_DEVICE}" -i "${RVS_SERVER_IP}" -u "${RVS_UDP_PORT}" -t "${RVS_CONTROL_PORT_1}" &
  export RVS_STREAM_PID=$!
}

rvs-embedded-start-all() {
  echo "Starting registration and streaming pipeline adb device, and its camera"
  trap rvs-cleanup INT TERM
  rvs-register-device && rvs-embedded-start-stream
}

rvs-local-start-all() {
  echo "Starting registration and streaming pipeline on this device, and this device's camera"
  trap rvs-cleanup INT TERM
  rvs-register-device && rvs-local-start-stream
}

rvs-cleanup() {
  echo "[${RVS_IDENTITY_NAME}] cleaning up…"
  kill "${RVS_STREAM_PID}" 2>/dev/null || true
  pkill -f "${RVS_BINARY}" 2>/dev/null || true
}

echo -e "\n####################################################################`
        `############"
echo "For normal use - rvs-start-all()"

echo -e "    rvs-register-device() \n`
    `       Registers on the given ${RVS_SERVER_IP}:4000/register \n`
    `       with the params given. Requests server to provision udp port \n`
    `       authenticated through RVS_SERVER_TOKEN - ${RVS_SERVER_TOKEN} \n"

echo -e "    rvs-embedded-start-stream() \n`
    `       Starts c app ${RVS_BINARY} on embedded device \n`
    `       to ${RVS_SERVER_IP}:<udp port>, where udp port is acquired through \n`
    `       rvs-register-device() \n"

echo -e "    rvs-local-start-stream() \n`
    `       Starts c app ${RVS_BINARY} on local device \n`
    `       to ${RVS_SERVER_IP}:<udp port>, where udp port is acquired through \n`
    `       rvs-register-device() \n"

echo -e "    rvs-listen-control()\n`
    `       Starts pythonic listener \n`
    `       for halt=1 on ${RVS_CONTROL_PORT_1} \n"

echo -e "    rvs-embedded-start-all() - \n`
    `       Registers with this device's connection, - \n`
    `       starts stream on embedded device through adb \n`
    `       🌑🌒🌓🌔🌕🌖🌗🌘"

echo -e "    rvs-local-start-all() - \n`
    `       Executes all commands in correct order locally - \n`
    `       registers on remote server, starts stream \n`
    `       🌑🌒🌓🌔🌕🌖🌗🌘"

echo "    rvs-cleanup() - Cleans up after your mess, be ashamed."

echo -e "\n####################################################################`
        `############"
