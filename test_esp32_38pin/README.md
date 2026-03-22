# ESP32 Industrial IoT Platform (PlatformIO)

**Production-Ready | Web & REST API | Industrial-Grade**

> ** Note:** This README combines both documentation files (README.md + README_INDUSTRIAL.md) into one comprehensive guide. README_INDUSTRIAL.md can be safely deleted. This file is the single source of truth.

[English] | **Bahasa Indonesia**

---

## Overview

ESP32 production-ready platform dirancang untuk **IoT fleet deployment**, **field technician management**, dan **remote monitoring**. Platform ini memungkinkan update konfigurasi tanpa reflash firmware, memberikan kontrol penuh melalui web interface dan REST API.

### Key Features

- **Web-based Configuration Management** — Update WiFi/IP konfigurasi dari lapangan, tanpa perlu reflash
- **REST API** — Complete API endpoints untuk monitoring dan kontrol dari mana saja
- **Admin Dashboard** — Professional web UI khusus untuk technician (user-friendly, no coding skills required)
- **NVS Persistent Storage** — Konfigurasi tersimpan otomatis, survive power loss & restart
- **Factory Reset** — Reset cepat untuk troubleshooting di lapangan
- **System Monitoring** — Real-time device health: free heap, uptime, WiFi signal
- **Non-Blocking Architecture** — Async API endpoints, smooth user experience
- **Gzip Compression** — Automatic asset compression untuk bandwidth efficiency

---

## Project Structure

Setiap folder dan file memiliki fungsi spesifik:

```
test_esp32_38pin/
├── include/
│   ├── credentials.h       ← Default config values (SSID, password, static IP)
│   ├── nvs_manager.h       ← NVS Storage abstraction layer (get/set/reset)
│   └── config_api.h        ← REST API endpoints handler (all 4 endpoints)
├── src/
│   └── main.cpp            ← Main application entry point & orchestration
├── data/
│   ├── index.html          ← Home page (simple + link to admin)
│   ├── admin.html          ← Admin panel for technicians (full control UI)
│   ├── style.css           ← Dark theme stylesheet (Catppuccin colors)
│   ├── script.js           ← Client-side JavaScript (form handling, API calls)
│   └── *.gz                ← Auto-compressed assets (gzip format)
├── platformio.ini          ← Build config (ESP32 board, libraries, settings)
├── compress_data.py        ← Python script to gzip-compress web assets
├── README_INDUSTRIAL.md    ← Previous industrial documentation (can be removed)
└── README.md               ← This comprehensive guide
```

### File Descriptions

| File               | Purpose             | Key Content                                              |
| ------------------ | ------------------- | -------------------------------------------------------- |
| `credentials.h`    | Default boot config | Fallback WiFi SSID, password, static IP addresses        |
| `nvs_manager.h`    | Persistent storage  | Get/set/read config from ESP32 NVS (survives power loss) |
| `config_api.h`     | REST API layer      | 4 endpoints: GET/POST config, GET system, POST reset     |
| `main.cpp`         | Application core    | Boot sequence, WiFi setup, API registration, main loop   |
| `admin.html`       | Web dashboard       | Form fields, real-time monitoring, factory reset button  |
| `compress_data.py` | Build optimization  | Auto-gzip HTML, CSS, JS for bandwidth savings            |

---

## Quick Start (5-10 Minutes)

Complete deployment dari build hingga device ready di lapangan:

### Step 0: Initial Setup (First Time Only)

```bash
# 1. Build project & download semua dependencies
pio run

# 2. Rebuild IntelliSense (hilangkan red squiggles di VS Code)
Ctrl + Shift + P → PlatformIO: Rebuild IntelliSense Index
```

> **Note:** Pastikan terhubung ke internet saat first build untuk download libraries.

### Step 1: Compress Web Assets

Kompres HTML, CSS, JS untuk bandwidth efficiency:

```bash
python compress_data.py
```

**Output:**

```
Compressed: data\admin.html -> data\admin.html.gz
Compressed: data\index.html -> data\index.html.gz
Compressed: data\script.js -> data\script.js.gz
Compressed: data\style.css -> data\style.css.gz
Compression complete!
```

### Step 2: Upload Filesystem (Web Assets)

Upload web interface files ke device:

```bash
pio run --target uploadfs
```

**Expected output:**

```
Flash will be erased from 0x00290000 to 0x003effff...
Wrote 1441792 bytes (10557 compressed) at 0x00290000 in 7.9 seconds
===== [SUCCESS] =====
```

### Step 3: Upload Firmware (Application)

Flash aplikasi utama ke ESP32:

```bash
pio run --target upload
```

**Expected output:**

```
Wrote 882432 bytes (563211 compressed) at 0x00010000 in 14.9 seconds
===== [SUCCESS] =====
```

### Step 4: Monitor & Verify Boot

Buka serial monitor untuk lihat boot sequence:

```bash
pio device monitor --baud 115200
```

**Expected boot output:**

```
LittleFS OK
[NVS] Preferences initialized
Loaded Config - SSID: Zaki
Static IP: 10.225.180.100
Connecting to WiFi.
IP Address (Fixed): 10.225.180.100
[API] Config endpoints registered
[BOOT] Startup complete - restart feature enabled
Web server started!
========================================
API Endpoints:
  GET  /api/config    - Get current config
  POST /api/config    - Update config
  GET  /api/system    - System info
  POST /api/reset     - Factory reset
========================================
```

**Device ready!** Akses sekarang di browser.

---

### Quick Upload One-Liner

Untuk update cepat (setelah first setup):

```bash
python compress_data.py && pio run --target uploadfs && pio run --target upload
```

---

## Web Interfaces

Device menyediakan dua halaman web untuk akses berbeda:

### Home Page (Simple Demo)

**URL:** `http://10.225.180.100/`

**Purpose:** Landing page untuk demo sekaligus gateway ke admin panel

**Content:**

- Welcome title
- "Klik Saya" button (demo interactivity)
- Status display
- Link ke Admin Panel

**Who uses it:** End users, visitors, demo purposes

**Example:**

```
ESP32 Industrial Monitor
────────────────────────

Klik Saya

Admin Panel: Click here
```

---

### Admin Dashboard (Configuration & Monitoring)

**URL:** `http://10.225.180.100/admin.html`

**Purpose:** Complete control center untuk device configuration & monitoring

**Features:**

#### Configuration Section

```
WiFi SSID:        [_____________________]
WiFi Password:    [_____________________]
Static IP:        [__].[ __ ].[  ].[ ___]
                  10   225    180   100

[ Simpan & Restart]  [ Factory Reset]
```

**Actions:**

- Update WiFi credentials → device restart dalam 5 detik
- Change IP address → requires reboot
- Factory reset → wipe all config, reboot dengan defaults

#### Real-Time System Monitoring (Auto-refresh 10s)

```
Device Information:
├─ Device Name:    ESP32 DOIT DevKit V1
├─ IP Address:     10.225.180.100
├─ WiFi Network:   Zaki
├─ MAC Address:    a0:b7:65:49:54:20
├─ Free Heap:      184.3 KB
└─ Uptime:         1 hour 5 minutes
```

**Color Coding:**

- Green: OK (heap > 50KB)
- Yellow: Warning (heap 20-50KB)
- Red: Critical (heap < 20KB)

#### Status Messages (Auto-clear after 5 sec)

- "Config updated. Device will restart in 5 seconds..."
- "Error: Invalid IP address format"
- "Device rebooting..."

**Responsive Design:**

- Works on desktop browsers
- Works on mobile browsers (smartphone)
- Works on tablets
- Dark theme (Catppuccin color scheme)

**Technician Quick Guide:**

```
How to change WiFi:
1. Open http://device-ip/admin.html on phone/laptop
2. Fill in WiFi SSID & password
3. Click "Simpan & Restart"
4. Wait 5 seconds
5. Done! Device connected ke WiFi baru
```

---

## Access from Different Networks

### Same Network (Recommended)

```
Device WiFi: Zaki
Your Phone: Connected to Zaki

→ Can access: http://10.225.180.100/admin.html
```

### Initial Setup (Temp Hotspot)

```
Device WiFi: Not yet connected (first boot)
Your Phone: Create hotspot "TempSetup"
Device: Connect to TempSetup hotspot

→ Can access: http://192.168.x.x/admin.html
→ Update WiFi to real network
→ Device reconnect to real network
```

### Remote Access (Cloud)

```
Device WiFi: Connected to Zaki (internet available)
Your Computer: Office/Home (different network)

→ Need: Cloud tunnel / VPN to access device
→ Or: Expose API with HTTPS (advanced)
```

---

## Configuration Management

Ada dua cara untuk konfigurasi device, masing-masing dengan use case berbeda:

### Method 1: Default Hardcoded Values (Development + Mass Deployment)

**Use case:**

- Development/testing
- Mass deployment dengan config bersama (contoh: 100 devices dengan WiFi sama)

**File:** `include/credentials.h`

```cpp
#define DEFAULT_WIFI_SSID "Zaki"              // Default WiFi network
#define DEFAULT_WIFI_PASS "zaki1234"          // Default password
#define DEFAULT_STATIC_IP_1 10                // 10.225.180.100
#define DEFAULT_STATIC_IP_2 225
#define DEFAULT_STATIC_IP_3 180
#define DEFAULT_STATIC_IP_4 100
#define DEFAULT_GATEWAY_1 10                  // Gateway: 10.225.180.1
#define DEFAULT_GATEWAY_2 225
#define DEFAULT_GATEWAY_3 180
#define DEFAULT_GATEWAY_4 1
#define DEFAULT_SUBNET_1 255                  // Subnet: 255.255.255.0
#define DEFAULT_SUBNET_2 255
#define DEFAULT_SUBNET_3 255
#define DEFAULT_SUBNET_4 0
```

**Cara update:**

1. Edit nilai di `credentials.h`
2. Rebuild & reflash: `python compress_data.py && pio run --target uploadfs && pio run --target upload`
3. Tidak diperlukan saat device sudah di lapangan

**Keuntungan:**

- Simple untuk development
- Cepat untuk mass deployment (flash sekali, ratusan devices)

**Kekurangan:**

- Perlu reflash jika ada perubahan
- Setiap device harus diedit config-nya saat deployment

### Method 2: Admin Dashboard (RECOMMENDED - Field Deployment)

**Use case:**

- Field technician update tanpa USB cable
- Change WiFi saat lokasi instalasi berubah
- Quick troubleshooting di lapangan

**Access:** `http://<device-ip>/admin.html`

**Fitur:**

- Update WiFi SSID & Password
- Modify Static IP Address (dengan 4 separate inputs)
- View System Info real-time (IP, MAC, memory, uptime)
- Factory Reset button
- Auto-refresh system monitoring setiap 10 detik

**Cara update:**

1. Buka browser → `http://10.225.180.100/admin.html`
2. Input WiFi SSID, Password, IP
3. Click " Simpan & Restart"
4. Device auto-restart dalam 5 detik
5. Config tersimpan permanent di NVS

**Keuntungan:**

- Tidak perlu reflash
- Update cepat dari smartphone/laptop
- Perfect untuk field deployment
- Config persistent (survive power loss)

**Kekurangan:**

- Perlu akses ke device IP

### Config Priority (Load Chain)

Saat device boot, config di-load dengan priority:

```
1. NVS Storage (user-configured via admin)
   ↓ (jika kosong, fallback ke:)
2. credentials.h (hardcoded defaults)
   ↓ (jika error, fallback ke:)
3. Hard-coded constants (IP 10.225.180.100)
```

**Artinya:**

- Field technician bisa override defaults via admin
- Jika admin config hilang (NVS corrupted), device masih bisa boot dengan credentials.h
- Safety mechanism untuk troubleshooting

---

## 🔌 REST API Documentation

Complete reference untuk mengelola device secara programmatic dari cloud/server/dashboard.

### API Base URL

```
http://10.225.180.100
```

Semua endpoint menggunakan `Content-Type: application/json`.

---

### GET /api/config

**Purpose:** Ambil konfigurasi saat ini dari device

**Request:**

```bash
curl http://10.225.180.100/api/config
```

**Response (Success):**

```json
{
  "wifi_ssid": "Zaki",
  "static_ip": "10.225.180.100"
}
```

**Response (Error - NVS empty):**

```json
{
  "wifi_ssid": "Zaki",
  "static_ip": "10.225.180.100"
}
```

> Note: Jika NVS kosong, akan return default values dari credentials.h

**Status Codes:**

- `200` - Success
- `404` - Endpoint not found
- `500` - Server error

---

### POST /api/config

**Purpose:** Update konfigurasi device (WiFi SSID, password, IP)

**Request Headers:**

```
Content-Type: application/json
```

**Request Body:**

```json
{
  "wifi_ssid": "NewNetwork",
  "wifi_pass": "SecurePassword123",
  "static_ip": "10.225.180.50"
}
```

**Curl Example:**

```bash
curl -X POST http://10.225.180.100/api/config \
  -H "Content-Type: application/json" \
  -d '{
    "wifi_ssid": "OfficeNetwork",
    "wifi_pass": "SecurePass2024",
    "static_ip": "192.168.1.100"
  }'
```

**Response (Success):**

```json
{
  "status": "OK",
  "message": "Config updated. Device will restart in 5 seconds..."
}
```

**Timeline setelah POST:**

```
T=0s    → POST received, config saved to NVS
T=1s    → "Restart scheduled in 5 seconds" logged
T=2s    → Response sent to client (200 OK)
T=5s    → Device auto-restart (non-blocking)
T=6s    → Device boot dengan config baru
T=10s   → Device reconnect ke WiFi baru
T=15s   → Device ready
```

**Error Responses:**

```json
{
  "error": "Invalid JSON"
}
```

→ Status: 400

**Notes:**

- Restart adalah **non-blocking** (tidak freeze device)
- Config di-save ke NVS sebelum restart
- Jika WiFi baru tidak connect, boot dengan default config
- API tidak bisa diakses selama first 10 seconds of boot untuk safety

---

### GET /api/system

**Purpose:** Monitor device health & status real-time

**Request:**

```bash
curl http://10.225.180.100/api/system
```

**Response:**

```json
{
  "device": "ESP32 DOIT DevKit V1",
  "ip": "10.225.180.100",
  "ssid": "Zaki",
  "mac": "a0:b7:65:49:54:20",
  "free_heap": 184320,
  "uptime": 3600
}
```

**Field Explanations:**
| Field | Type | Meaning | Unit |
|-------|------|---------|------|
| `device` | String | Model ESP32 | - |
| `ip` | String | Current IP address | - |
| `ssid` | String | Connected WiFi network | - |
| `mac` | String | MAC address (unique identifier) | - |
| `free_heap` | Number | Available RAM | bytes |
| `uptime` | Number | How long device running | seconds |

**Memory Interpretation:**

- `free_heap > 50000` = Healthy
- `free_heap 20000-50000` = Warning
- `free_heap < 20000` = Critical

**Curl with JSON parsing (requires jq):**

```bash
curl http://10.225.180.100/api/system | jq .
```

---

### POST /api/reset

**Purpose:** Factory reset device ke default configuration

**Request:**

```bash
curl -X POST http://10.225.180.100/api/reset
```

**Response (Success):**

```json
{
  "status": "OK",
  "message": "Factory reset. Device will restart..."
}
```

**What happens:**

1. Semua custom config di NVS dihapus
2. Device restart dalam 2 detik
3. Boot dengan credentials.h defaults
4. IP kembali ke DEFAULT (10.225.180.100)

**Timeline:**

```
T=0s → POST received
T=1s → Response sent (200 OK)
T=2s → Device restart
T=3s → Device boot dengan default config
T=5s → Device ready
```

**Use Cases:**

- Quick troubleshooting saat WiFi problem
- Setup ulang sebelum re-deploy ke lokasi baru
- Emergency recovery jika config corrupt

---

### API Error Handling

**Status Codes:**

| Code | Meaning      | Action                                        |
| ---- | ------------ | --------------------------------------------- |
| 200  | Success      | Config saved / system info retrieved          |
| 400  | Bad Request  | Invalid JSON format → check your request body |
| 404  | Not Found    | Wrong endpoint URL → check spelling           |
| 500  | Server Error | Device crash or memory issue → restart device |

**Common Errors & Solutions:**

```
Error: "Invalid JSON"
→ Check JSON syntax (missing quotes, commas, etc.)
→ Use: curl -X POST ... -d '{"key":"value"}'

Error: Connection timeout
→ Device not responding
→ Check device IP address
→ Restart device: POST /api/reset
→ Check WiFi connection on device

Error: 404 Not Found
→ Check endpoint spelling (case-sensitive)
→ Verify device IP is correct
```

---

## NVS (Non-Volatile Storage) Explained

**NVS** = Non-Volatile Storage adalah ESP32's built-in persistent memory untuk menyimpan konfigurasi yang **survive** power loss, restart, dan reboot.

### Storage Architecture

```
ESP32 Flash Memory Layout:
┌─────────────────────┐
│ Bootloader          │ 0x1000-0x8000
├─────────────────────┤
│ Partition Table     │ 0x8000-0x9000
├─────────────────────┤
│ OTA (optional)      │ 0x9000-0x10000
├─────────────────────┤
│ Firmware            │ 0x10000-0xe7fff (882KB)
├─────────────────────┤
│ LittleFS (Web)      │ 0x290000-0x3effff (1.4MB)
├─────────────────────┤
│ NVS (Config)      │ (Reserved partition)
└─────────────────────┘
```

### Storage Table

Config disimpan di **NVS namespace**: `esp32_config`

| Key         | Type   | Size | Example Value    | Notes             |
| ----------- | ------ | ---- | ---------------- | ----------------- |
| `wifi_ssid` | String | 32B  | "Zaki"           | WiFi network name |
| `wifi_pass` | String | 64B  | "zaki1234"       | WiFi password     |
| `static_ip` | String | 20B  | "10.225.180.100" | IP address format |

### Data Persistence & Lifetime

**Persisten (tidak terhapus saat):**

- Device restart (ctrl+alt+del tidak ada, tapi soft-reset ada)
- Power loss & reboot
- WiFi disconnect/reconnect
- Software crash & auto-restart

**Terhapus saat:**

- Factory reset (`POST /api/reset`)
- Erase flash memory (`pio run --target erase`)
- Manual NVS clear (advanced)

### How Config is Loaded

**Boot sequence:**

```
T=0         → Device boot
T=50ms      → Initialize LittleFS & NVS
T=100ms     → Read NVS namespace "esp32_config"
            ↓
            Cek NVS keys ada atau tidak?
            ↓
T=110ms     ├─ YES → Load dari NVS (user config)
            │        ↓
            │        Use SSID: "CustomNetwork", IP: "192.168.1.5"
            │
            └─ NO  → Load dari credentials.h (defaults)
T=120ms              ↓
                     Use SSID: "Zaki", IP: "10.225.180.100"
```

### Code Location

Implementation ada di [include/nvs_manager.h](include/nvs_manager.h):

```cpp
// Read config
String ssid = configMgr.getWiFiSSID();        // → dari NVS atau default

// Write config
configMgr.setWiFiSSID("NewNetwork");          // → save ke NVS permanent

// JSON representation
String json = configMgr.toJSON();             // → {"wifi_ssid":"..."}

// Factory reset
configMgr.resetToDefaults();                  // → clear NVS namespace
```

---

## How Restart Mechanism Works

Device dapat di-restart dalam dua situasi: auto-restart setelah config update, atau manual factory reset. Mekanisme restart dirancang **non-blocking** agar web server tetap responsive.

### Non-Blocking Restart Architecture

**Why non-blocking?**

- Jika menggunakan `delay(5000)` sebelum ESP.restart(), web server akan freeze
- Client akan timeout waiting untuk response
- Tidak bisa handle request lain selama delay

**Solution: Scheduled Restart via Timer**

**Code Location:** [include/config_api.h](include/config_api.h) + [src/main.cpp](src/main.cpp)

**Variables:**

```cpp
static volatile unsigned long restartTime = 0;  // When to restart (epoch millis)
static volatile bool bootComplete = false;      // Can restart after boot done?
```

**Timeline Example (POST /api/config):**

```
T=0ms   ├─ Browser sends POST request
        ├─ Data received on device
T=10ms  ├─ Parse JSON, validate data
        ├─ Save to NVS
T=20ms  ├─ Set: restartTime = millis() + 5000
        ├─ Send response to browser IMMEDIATELY
T=30ms  ├─ Browser receives: {"status":"OK","message":"Will restart in 5s..."}
T=30-5000ms: Loop continues normally
        ├─ Web server still responsive
        ├─ Can handle other requests
T=5000ms├─ Loop checks: millis() >= restartTime? YES
        ├─ Execute: ESP.restart()
T=5050ms├─ Device reset via watchdog
T=5100ms├─ Boot sequence starts
T=5500ms├─ WiFi connects with new config
T=5800ms├─ Web server ready
```

**Benefits:**

- Response sent immediately (user feedback instant)
- Web server never freezes
- Other requests can be handled before restart
- Smooth user experience

### Safety Features

**1. Boot Protection (first 10 seconds)**

Saat boot, restart feature di-disable untuk mencegah false-restart:

```cpp
if (millis() < 10000) {
    return;  // Don't restart during first 10 seconds
}
```

**Why?** Browser auto-loading admin.html saat boot bisa trigger POST requests, potentially causing infinite restart loop.

**2. Boot Completion Flag**

Setup() hanya set flag setelah everything ready:

```cpp
// At end of setup()
ConfigAPI::bootCompleted();  // Enable restart feature
```

Serial output:

```
[BOOT] Startup complete - restart feature enabled
```

**3. Thread-Safe Variables**

```cpp
volatile unsigned long restartTime = 0;  // volatile = compiler tidak cache value
```

Volatile memastikan variable selalu dibaca dari memory, tidak dari register.

### Monitoring via Serial

**Example output saat config update via /api/config:**

```
[ 14651] User opens admin panel
[ 15000] [API] Restart scheduled in 5 seconds...
[ 15500] [API] Scheduled restart executing...
ets Jul 29 2019 12:21:46
rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
...
LittleFS OK
[NVS] Preferences initialized
Loaded Config - SSID: NewNetwork
Static IP: 192.168.1.100
Connecting to WiFi.
IP Address (Fixed): 192.168.1.100
[API] Config endpoints registered
[BOOT] Startup complete - restart feature enabled
Web server started!
```

---

## Real-World Use Cases

Bagaimana platform ini digunakan di berbagai scenario:

### Use Case 1: Mass Deployment (100+ Devices)

**Scenario:** Perusahaan ingin deploy 100 device ESP32 ke lokasi berbeda, setiap lokasi punya WiFi network berbeda.

**Traditional Approach (Buruk ):**

```
1. Edit credentials.h dengan WiFi #1 → Reflash device #1
2. Edit credentials.h dengan WiFi #2 → Reflash device #2
3. ... repeat 100 times
 Total waktu: 10-15 jam
```

**Platform Approach (Baik ):**

```
1. Flash semua 100 device dengan default config (1 jam, parallelize)
2. Di lapangan, technician buka admin panel (http://device-ip/admin.html)
3. Update WiFi SSID/password sebelum instalasi (30 detik per device)
4. Device auto-restart → connect ke WiFi lokal
 Total field time: 1 jam (vs 15 jam dengan reflash!)
```

**Savings:** 93% waktu deployment!

---

### Use Case 2: Field Technician WiFi Update

**Scenario:** Lokasi instalasi berubah, WiFi network lama sudah disconnect. Technician perlu update ke WiFi baru.

**Traditional Approach (Buruk ):**

```
1. Technician bawa laptop + USB cable ke lokasi
2. Connect device via USB → find correct COM port
3. Edit credentials.h → compile → upload (5 menit)
4. Serial monitor → verify boot
 Total time: 10 menit per device
```

**Platform Approach (Baik ):**

```
1. Technician punya hanya smartphone
2. Buka browser → http://device-ip/admin.html (automatic)
3. Fill form: WiFi SSID, password
4. Click "Simpan & Restart"
5. Wait 5 seconds → device reconnect ke WiFi baru
 Total time: 30 seconds per device
```

**Requirements:**

- Technician doesn't need: Laptop, USB cable, coding skills
- Device connected ke any available WiFi temporarily

**Workflow:**

```
Initial state: Device punya hotspot dari mobile phone
                ↓
Technician akses http://192.168.1.1/admin.html (temp IP)
                ↓
Fill WiFi credentials dari network lokal
                ↓
Click save → device restart
                ↓
Device disconnect dari hotspot → connect ke network lokal
                ↓
Device ready untuk use (tech dapat new IP dari admin)
```

---

### Use Case 3: Remote Monitoring & Troubleshooting

**Scenario:** Company has 50 devices di lapangan. Manager ingin monitor semua device dari office/cloud.

**Setup:**

```
Cloud Server (office)      Device #1
    │                          │
    └──── HTTPS Request ───────┤
         GET /api/system      │
         ←─── JSON Response ──┤
```

**Monitoring Script:**

```bash
#!/bin/bash
# Monitor all devices every 5 minutes

for device_ip in 10.225.180.{100..150}; do
  response=$(curl -s http://$device_ip/api/system)

  free_heap=$(echo $response | jq .free_heap)
  uptime=$(echo $response | jq .uptime)

  # Alert if memory low
  if [ $free_heap -lt 20000 ]; then
    echo " Device $device_ip: Low memory! Heap=$free_heap"
    # Send alert email / Slack notification
  fi

  # Log to database
  echo "$(date),Device=$device_ip,Heap=$free_heap,Uptime=$uptime" >> devices.log
done
```

**Dashboard View:**

```
Device Monitoring Dashboard
┌──────────────────────────────────────────────────┐
│ Device #1 (10.225.180.100)                       │
│ ├─ WiFi: Zaki (signal: -45dBm)                   │
│ ├─ IP: 10.225.180.100                            │
│ ├─ Free Heap: 184KB  (Healthy)                 │
│ ├─ Uptime: 3600s (1 hour)                        │
│ └─ Last seen: 30 seconds ago                   │
│                                                  │
│ Device #2 (10.225.180.101)                       │
│ ├─ WiFi: Zaki (signal: -60dBm)                   │
│ ├─ IP: 10.225.180.101                            │
│ ├─ Free Heap: 18KB  (Warning: Low)             │
│ ├─ Uptime: 7200s (2 hours)                       │
│ └─ Last seen: 2 minutes ago                    │
│                                                  │
│ Device #3 (10.225.180.102)                       │
│ ├─ Status:  OFFLINE                            │
│ └─ Last seen: 45 minutes ago                   │
└──────────────────────────────────────────────────┘

Alert: Device #3 offline for 45 minutes → Check connection
```

**Benefits:**

- Proactive monitoring (catch issues before user complains)
- Fleet health dashboard
- Memory/resource tracking
- Uptime statistics
- Historical data for analysis

---

## Deployment Checklist

- [ ] Dependencies ter-download (`pio run`)
- [ ] IntelliSense updated (Rebuild index)
- [ ] Default config di `credentials.h` sesuai
- [ ] Static IP unique (tidak bentrok)
- [ ] Assets ter-compress (`python compress_data.py`)
- [ ] Filesystem ter-upload (`pio run --target uploadfs`)
- [ ] Firmware ter-upload (`pio run --target upload`)
- [ ] Serial Monitor: verify boot sequence
- [ ] Access home page: `http://device-ip`
- [ ] Access admin panel: `http://device-ip/admin.html`
- [ ] Test WiFi config update via admin
- [ ] Device label dengan IP address

---

## Device Label Template

Cetak & tempel di device untuk field technician reference:

```
╔════════════════════════════════════╗
║   ESP32 Industrial Monitor v1.0    ║
║                                    ║
║  IP Address:   10.225.180.100      ║
║  Admin Panel:  /admin.html         ║
║  API Base:     /api/                ║
║                                    ║
║  WiFi: [Update via admin panel]    ║
║                                    ║
║   Do not reset unless instructed ║
╚════════════════════════════════════╝
```

---

## Troubleshooting

### Build Error: "ArduinoJson not found"

```bash
# Clean & rebuild
pio run --target clean
pio run
```

### Device tidak connect ke WiFi

**Debug steps:**

1. Buka Serial Monitor: `pio device monitor --baud 115200`
2. Cek error messages
3. Verify WiFi SSID & password via `/admin.html`
4. Check WiFi signal strength di lokasi
5. Try factory reset: `POST /api/reset`

### Config tidak tersimpan

**Causes:**

- LittleFS not uploaded (`pio run --target uploadfs`)
- NVS corrupted → factory reset
- Insufficient flash space

**Fix:**

```bash
pio run --target uploadfs  # Re-upload filesystem
```

### Admin panel page loading lama/error

- Check browser console untuk error
- Verify device IP address
- Try hard refresh (Ctrl+Shift+R)
- Check free heap via `/api/system`

---

##  Libraries & Dependencies

| Library                    | Version | Purpose           |
| -------------------------- | ------- | ----------------- |
| **ESPAsyncWebServer**      | ^3.10.3 | Async web server  |
| **ArduinoJson**            | ^7.0.0  | JSON handling     |
| **LittleFS**               | 2.0.0   | Filesystem        |
| **WiFi** (built-in)        | -       | WiFi connectivity |
| **Preferences** (built-in) | -       | NVS wrapper       |

---

## Technical Notes

- **Config Load:** NVS → fallback ke defaults (`credentials.h`)
- **WiFi Retry:** 20 attempts (10 detik timeout)
- **Restart Delay:** 5 detik setelah config update
- **NVS Namespace:** `esp32_config`
- **Baud Rate:** 115200
- **Static IP:** Default 10.225.180.100 (adjustable)

---

##  Production Recommendations

### Essential (Before Deploy)

- Use unique Static IP per device
- Secure default password (`credentials.h`)
- Test factory reset procedure
- Verify all API endpoints

### For Enhanced Security (Optional)

-  Add HTTP Basic Auth to `/api/`
-  Encrypt passwords di NVS
-  Implement HTTPS dengan self-signed cert
-  Add OTA (Over-The-Air) firmware update

### For Fleet Management (Advanced)

-  Cloud sync untuk centralized config
- Logging & event tracking
- Automated rollback on error
- Mobile app untuk management

---

## Next Steps

1. **Deploy device ke lapangan**
2. **Print & attach label** (device info)
3. **Train technician** (admin panel usage)
4. **Setup monitoring** (check `/api/system` periodically)
5. **Document deployment** (MAC addresses, IPs, locations)

---

##  Support & Issues

- **Build issues?** → Check `platformio.ini` dependencies
- **WiFi problems?** → Use admin panel to reconfigure
- **API not working?** → Check free heap & logs
- **Need factory reset?** → POST `/api/reset`

---

##  License & Credits

Built with:

- PlatformIO IDE
- ESP32 SDK
- Arduino Framework
- ESPAsyncWebServer

---

** Ready for production deployment!**

Last Updated: 2026-03-22
