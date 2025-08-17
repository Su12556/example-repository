## Smallest ESP32 AI Drone (65mm Whoop)

Build a palm-sized brushless whoop-class drone powered by an ESP32-S3, with optional on-device AI (gesture/vision). This repo gives you a minimal, understandable flight stack you can grow from, not a full Betaflight clone.

- **Size target**: 65 mm whoop
- **MCU**: ESP32-S3 (for vector ops + optional camera)
- **ESC**: 1S 4-in-1 BLHeli_S/32
- **Motors**: 0702–0802, 1S
- **IMU**: ICM-42688-P (or MPU-6050 as a drop-in fallback)
- **Control link**: ESP-NOW RC (simple), or integrate SBUS/CRSF later
- **AI**: Optional TinyML/ESP-DL modules (gesture/obstacle cues)

### Safety and legality
- **Spinning props are dangerous.** Remove props until your arming logic, orientation, and failsafes are verified.
- Check local regulations for UAV operation. Keep <250 g and fly responsibly.

### Repo layout
- `hardware/` — BOM and wiring for a minimal build
- `firmware/esp-idf/` — ESP-IDF project (PID, IMU, mixer, LEDC PWM driver, ESP-NOW RX)
- `ai/` — notes and hooks for later AI features (ESP-DL/TinyML)

### What works now
- LEDC PWM motor driver (1000–2000 µs) for ESCs in PWM mode
- Simple complementary-filter attitude estimation
- PID-based stabilization and quad-X mixer
- ESP-NOW receiver stub (ready for a simple transmitter)

DShot is sketched but not enabled by default; start with PWM so you can test on the bench.

### Hardware snapshot
See `hardware/BOM.md` for exact parts and `hardware/wiring.md` for pin mapping.

- ESP32-S3 module (e.g., ESP32-S3-MINI-1) or small dev board
- 1S 4-in-1 whoop ESC (5–12 A) that supports PWM mode
- IMU: ICM-42688-P (SPI/I2C). Fallback: MPU-6050 (I2C)
- 1S LiPo 300–450 mAh. 3.3 V regulator if your ESC has no BEC

### Build the firmware
Prereqs:
- ESP-IDF v5.x installed and exported to your shell (`idf.py --version`)
- Target MCU: ESP32-S3 (you can adapt to other ESP32 variants)

Commands:
```bash
cd firmware/esp-idf
idf.py set-target esp32s3
idf.py build
# Put board in download mode, then:
idf.py -p /dev/ttyUSB0 flash monitor
```

If you use a different ESP32 family, update the target and check `config.h` pins.

### Configure pins and options
Edit `firmware/esp-idf/main/config.h`:
- Motor pins `MOTOR_GPIO_[1..4]`
- I2C pins for IMU `I2C_SDA_GPIO`, `I2C_SCL_GPIO`
- PWM range `MOTOR_MIN_US`/`MOTOR_MAX_US`
- Loop rate, PID gains

### First bring-up (no props)
1. Flash firmware; open serial at 115200.
2. Verify IMU detected and attitude numbers move when tilting.
3. Send ESP-NOW RC packets (or use the provided test pattern) and confirm motor outputs change in the log.
4. Validate failsafe: on link-loss, motors go to minimum.

### Roadmap
- DShot300/600 via RMT for BLHeli_S/32
- RX: CRSF/SBUS input
- AI: gesture control or simple obstacle prompts with ESP-DL (OV2640)
- Telemetry over ESP-NOW/BLE

### Disclaimer
This is a reference and learning project. Fly in safe, controlled environments, and assume responsibility for any damage or injury.
