````markdown
# Sample README

**Hint**: source `setenv.sh`

## 1. Valid Stream Acquisition

```c
// Your stream acquisition code here
````

---

## 2. Communications and Data Transfer

Test numero uno. For the reader:

```c
// Your communication logic here
```

---

## 3. Visualization and Security

```c
// Your visualization/security code here
```

---

## 4. Server

### 4.1 Registration

Web UI `POST /register` → auth via `DEVICE_KEY` → stores `{ name, udp, ctrl1, ctrl2, ip }` in SQLite → returns assigned UDP port.

### 4.2 Stream Ingestion

Bridge polls `store.db`, for each camera UDP port:

```bash
ffmpeg -i udp://0.0.0.0:<udp>?listen=1 -f flv rtmp://localhost:1935/<name>
```

### 4.3 Streaming

* **RTMP**: `rtmp://<host>:1935/<name>` (use in VLC/OBS)
* **HLS**: `http://<host>:8888/<name>/index.m3u8` (browser via HLS.js)

### 4.4 Control Commands

Control commands allow frontend users to send instructions one-by-one via the `/cmd` endpoint. Each slider or custom input maps to a UDP message defined in `web/control.json`.

Requirements for successful commands:

1. Camera must be reachable at the IP used for `/register`.
2. UDP ports must be forwarded and firewalls configured.

### 4.5 Setup & Deployment

1. **Clone repository**

   ```bash
   git clone <repo-url> && cd <repo-root>
   ```

2. **Configure environment**

   ```bash
   export JWT_SECRET=your_jwt_secret
   export ADMIN_PASSWORD=secure_admin_pass
   export DEVICE_KEY=register123
   export BASE_UDP=5001
   ```

3. **Start services**

   ```bash
   docker-compose up --build -d
   ```

   * `auth` on port **3000**
   * `media` (MediaMTX) RTMP on **1935**, HLS on **8888**
   * `bridge` reads `/data/store.db`
   * `web` on port **4000**

4. **Access UI**
   Visit `http://<host>:4000`, login with `admin` / `ADMIN_PASSWORD`.

5. **Register your camera**
   Use `/register` or the script `scripts/host_scripts.sh`:

   ```bash
   curl -X POST http://<host>:4000/register \
     -H "Content-Type: application/json" \
     -d '{"key":"register123","name":"cam1","ctrl1":6001,"ctrl2":6002}'
   ```

   * `key`: your DEVICE\_KEY
   * `name`: unique camera identifier
   * `ctrl1`, `ctrl2`: control ports on the device

### 4.6 Teardown & Rebuild

* **Stop & remove all services**

  ```bash
  docker-compose down -v
  ```

  `-v` removes the `store.db` volume.

* **Rebuild & restart**

  ```bash
  docker-compose up --build -d
  ```

### 4.7 Customizing Controls

Edit `web/control.json`:

```json
[
  { "lbl": "Brightness", "cmd": "b", "min": -10, "max": 10 },
  { "lbl": "Contrast",   "cmd": "c", "min": 0,   "max": 20 }
]
```

Then restart the web service:

```bash
docker-compose restart web
```

```
```
