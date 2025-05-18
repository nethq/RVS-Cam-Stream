This is a sample readme file.
Fill in sections that corespond to your part of the assignment.
Feel free to rename sections.

## HINT - source setenv.sh

# 1. Valid Stream acquisition:
    /* ... */

---
# 2. Communications and data transfer:
test numero uno. For the girl reading this
    /* ... */

---
# 3. Visualization and security:
    /* ... */

---
# 4. Server

## 4.1. Registration
Web UI `POST /register` → auth via `DEVICE_KEY` → stores `{ name, udp, ctrl1, ctrl2, ip }` in SQLite → returns assigned `udp` port.
## 4.2. Stream Ingestion
Bridge polls `store.db`, for each camera `udp` port:
`ffmpeg -i udp://0.0.0.0:<udp>?listen=1 -f flv rtmp://localhost:1935/<name>`
## 4.3. Streaming
* RTMP: `rtmp://<host>:1935/<name>` (use in VLC/OBS)
* HLS: `http://<host>:8888/<name>/index.m3u8` (browser via HLS.js)
## 4.4. Control Commands
Control Commands alllow frontend users to send commands to the backend devices. Using /cmd endpoint, commands are sent one by one,
Commands on the frontend are translated into their expected syntax by the C APP through `control.json` in the `/web` module on submit.
For commands to work on your device, it needs to be reachable through the IP used to hit `<server>:4000/register`
Which in most cases happens to be the WAN IP. I.e the user needs to manually port forward the ports, and regulate firewalls

## 4.5 Setup & Deployment

- **Clone repo**
- **Configure env**
In your shell set the following:

```bash
 JWT_SECRET=your_jwt_secret
 ADMIN_PASSWORD=secure_admin_pass
 DEVICE_KEY=register123
 BASE_UDP=5001
```

- **Start all services**

```bash
docker-compose up --build -d
```
* `auth` on **3000**
* `media` (MediaMTX) RTMP on **1935**, HLS on **8888**
* `bridge` reads `/data/store.db`
* `web` on **4000**

- **Access UI**
Visit `http://<host>:4000` in your browser, and login with `admin`/`ADMIN_PASSWORD` (var passed in docker-compose.yml) (admin/admin default).

- **Register your camera**
Registration of the camera happens on the /register endpoint.
`scripts/host_scripts.sh` defines a curl-based registration script.
The required parameters are `{ key, name, ctrl1, ctrl2 }` in json format.
Where:
1. `key` is the authentication key that the backend was built with (env var) 
2. `name` unique name for the camera (will appear in the frontend)
3. `ctrl1`,`ctrl2` are the control ports that the host expects traffic on.    

## 4.6 Annihilation of Server & Rebuild

To stop and remove everything:

```bash
docker-compose down -v
```

* `-v` also deletes `store.db` volume.
  To rebuild from scratch:

```bash
docker-compose up --build -d
```

## Customizing Controls

Edit `web/control.json` to tweak parameters:

```json
[
  { "lbl": "Brightness", "cmd": "b", "min": -10, "max": 10 },
  { "lbl": "Contrast",   "cmd": "c", "min": 0,   "max": 20 }
]
```

Restart `web` service to apply changes.
```bash
docker compose down web
docker compose up web -d
```

