## Wiring (ESP32-S3 → Whoop ESC + IMU)

The mapping below matches the default `firmware/esp-idf/main/config.h`.

### Power
- Battery 1S → ESC main pads
- If ESC has BEC 5 V only: add a 5 V→3.3 V regulator for ESP32-S3 and IMU
- If ESC has 3.3 V BEC (rare): power ESP32-S3 and IMU from 3.3 V BEC
- Common GND between MCU, IMU, and ESC

### Motors (PWM initially; DShot later)
- `GPIO2`  → Motor 1 (front-left)
- `GPIO3`  → Motor 2 (front-right)
- `GPIO4`  → Motor 3 (rear-right)
- `GPIO5`  → Motor 4 (rear-left)

Use short twisted pairs for signal+GND to each ESC input. Start with PWM; switch to DShot once everything is stable.

### IMU (I2C)
- `GPIO8`  → SDA
- `GPIO9`  → SCL
- 3.3 V and GND to IMU

Default IMU is MPU-6050 (0x68) as a fallback. For ICM-42688, keep I2C wiring and update the driver later, or switch to SPI.

### Optional peripherals
- ToF (VL53L0X): I2C on same bus (change address if needed)
- Camera (OV2640): Use ESP32-S3 8-bit camera interface (requires different pins/board)

### Orientation
Quad-X convention used by the mixer:
- M1: front-left
- M2: front-right
- M3: rear-right
- M4: rear-left

Ensure motor direction and prop orientation match the standard whoop pattern (alternate CW/CCW). Reverse in ESC if needed.