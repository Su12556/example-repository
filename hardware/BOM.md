## BOM (65 mm ESP32-S3 Whoop)

- **MCU**: ESP32-S3-MINI-1 (module) or a small ESP32-S3 dev board
  - Why: small, dual-core, vector accel, optional camera support
- **IMU**: ICM-42688-P (SPI/I2C) or BMI270; fallback: MPU-6050 (I2C)
- **ESC**: 1S 4-in-1 whoop ESC, 5–12 A, supports PWM mode (or DShot later)
- **Motors**: 0702–0802 brushless, 1S, 19000–28000 KV
- **Props**: 31–40 mm whoop props
- **Battery**: 1S LiPo 300–450 mAh (PH2.0 or BT2.0)
- **Regulator**: 3.3 V buck/linear regulator (if ESC has no 3.3/5 V BEC)
  - Example footprints: ME6211/AMS1117 (LDO), MP1589 or similar buck
- **Level shifting**: Not needed (all 3.3 V). Ensure IMU is 3.3 V-compatible.
- **Frame**: 65 mm whoop frame with canopy
- **Fasteners**: M1.4/M2 screws as required
- **Optional sensors**: VL53L0X ToF (I2C), barometer (BMP280), magnetometer (QMC5883)
- **Optional vision**: OV2640 camera + FPC breakout (for ESP32-S3 camera interface)
- **Cables/Connectors**: JST-SH/Molex as needed, silicone wire 28–30 AWG

Notes:
- For the truly smallest footprint, design a custom PCB that hosts the ESP32-S3 module, IMU, level-reg, and brings out 4 motor pads to the 4-in-1 ESC.
- If your ESC does not accept PWM, enable DShot in firmware and wire digital signals to each ESC input.