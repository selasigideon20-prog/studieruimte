# Pinout

## Sensoren
- IN_SENSOR -> GPIO 4
- OUT_SENSOR -> GPIO 19
- SOUND_SENSOR (AO) -> GPIO 2

## Outputs
- RED_LED -> GPIO 21
- GREEN_LED -> GPIO 20
- BLUE_LED -> GPIO 22
- BUZZER -> GPIO 8

## Input
- BUTTON -> GPIO 10 (INPUT_PULLUP)

## LCD (I2C 16x2)
- SDA -> GPIO 6
- SCL -> GPIO 7
- VCC -> 5V
- GND -> GND


## Voeding
- ESP32 -> 5V via USB
- Sensoren -> 5V 
- Alle GND verbonden samen
