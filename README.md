# Embedded Systems Journey

A hands-on learning repository containing lab projects in embedded systems and IoT. Projects progress from foundational concepts (GPIO, sensors, communication protocols) to advanced topics (real-time systems, network management, cloud integration, REST APIs, and production deployment strategies).

## 🎯 Goals

- Document my learning journey in embedded systems development
- Implement full-stack IoT solutions (firmware → hardware → API → UI)
- Share production-ready code patterns and best practices
- Build reusable components for microcontroller-based projects

## 📚 Projects

### test_esp32_38pin
ESP32 development platform with WiFi configuration, IP management, and web-based admin panel. Features REST API for device configuration, NVS persistent storage, and validation workflows for critical operations.

**Key Features:**
- WiFi SSID/password management with secure storage
- Static IP configuration with validation & conflict detection
- REST API endpoints for device management
- Web-based admin panel (dark theme)
- Non-blocking boot protection & restart scheduling
- Over-The-Air (OTA) firmware update preparation

**Tech Stack:** Arduino + PlatformIO, ESPAsyncWebServer, ArduinoJson, NVS (Preferences)

## 🛠️ Tech Stack

- **Microcontrollers:** ESP32, ARM Cortex-M (expandable)
- **Framework:** Arduino + PlatformIO
- **Languages:** C++, JavaScript, HTML/CSS
- **Tools:** Git, PlatformIO CLI, Arduino IDE, Serial debugging

## 📖 Learning Path

1. **Basics:** GPIO control, analog I/O, interrupts
2. **Communication:** UART, SPI, I2C protocols
3. **Sensors & Actuators:** Integration patterns, calibration
4. **Networking:** WiFi, TCP/IP, REST APIs
5. **Storage:** NVS, SPIFFS, data persistence
6. **Real-time Systems:** Scheduling, state machines, interrupt handling
7. **Production:** Validation, error handling, monitoring, OTA updates

## 📦 Project Structure

```
Embedded-Systems-Journey/
├── test_esp32_38pin/        # Main ESP32 project
│   ├── src/                 # Firmware source
│   ├── include/             # Header files (config_api.h, credentials.h, etc.)
│   ├── lib/                 # Custom libraries
│   ├── data/                # Web assets (admin.html, CSS)
│   ├── test/                # Unit tests
│   └── platformio.ini       # PlatformIO configuration
├── README.md                # This file
└── docs/                    # Technical documentation (future)
```

## 🚀 Getting Started

### Prerequisites
- PlatformIO CLI or IDE
- ESP32 development board
- USB cable for serial communication

### Compile & Upload
```bash
cd test_esp32_38pin
pio run                      # Compile
pio run --target uploadfs    # Upload filesystem (web assets)
pio run --target upload      # Upload firmware
```

### Access Admin Panel
1. Power on ESP32
2. Connect to WiFi SSID: `MCU_CONFIG_AP` (password: `12345678`)
3. Open browser: `http://10.225.180.100`
4. Configure WiFi, IP, and other settings

## 📝 Documentation

Each project includes:
- `README.md` - Project overview & setup instructions
- `include/` - API documentation via code comments
- Technical notes on design decisions and lessons learned

## 🔍 Key Learnings

### IP Management
- Implemented validation-before-action pattern to prevent IP conflicts
- Non-blocking state machine for safe configuration changes
- Comprehensive error handling (subnet checks, gateway/broadcast rejection)

### Production Patterns
- Boot protection flag to prevent incomplete configurations
- Non-blocking restart scheduler (prevents task watchdog triggers)
- Secure credential storage using NVS Preferences

## 📈 Metrics

- **Projects:** 1+ (expanding)
- **Total Code:** 2000+ lines (firmware + web)
- **IoT Protocols:** WiFi (802.11b/g/n)
- **API Endpoints:** 5+ (config, system info, OTA, validation)

## 🤝 Contributing

These are personal learning projects. Feel free to fork, adapt, and use code patterns in your projects!

## 📄 License

MIT License - See LICENSE file for details

## 👋 About

I'm a Junior Embedded Systems/IoT Engineer learning full-stack development from hardware to cloud. This repository documents my journey and serves as a knowledge base for future reference.

**Connect:** [GitHub Issues](https://github.com/Zaki-Fadlan/Embedded-Systems-Journey/issues) for questions or discussions.

---

*Last Updated: March 2026*
