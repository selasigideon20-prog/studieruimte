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

## LCD (16x2)
- RS -> GPIO 12
- E  -> GPIO 5
- D4 -> GPIO 6
- D5 -> GPIO 7
- D6 -> GPIO 15
- D7 -> GPIO 18

## LCD extra aansluitingen
- VSS -> GND
- VDD -> 5V
- RW  -> GND
- VO  -> potentiometer (contrast)

## Backlight LCD
- A (LED+) -> 5V
- K (LED-) -> GND

## Voeding
- ESP32 -> 5V via USB
- Sensoren -> 3.3V of 5V (afhankelijk van module)
- Alle GND verbonden samen
