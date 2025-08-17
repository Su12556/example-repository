## AI Features (Optional)

This project leaves hooks for simple onboard AI on ESP32-S3:

- Gesture control from camera or IMU sequences (TinyML)
- Obstacle cues (left/right/forward) from a tiny CNN with ESP-DL + OV2640

Getting started:
- Use ESP-IDF v5.x with `esp-nn` and `esp-dl`. See Espressif examples for S3 camera.
- Start with low-res (96×96 or 160×120) grayscale inference and a 3-class output.
- Drive high-level cues into the flight controller as small roll/pitch/yaw offsets with strong clamping.

Safety: keep AI outputs bounded and blended at low gain so manual control dominates. Add a hard switch to disable AI mixing.