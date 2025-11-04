# Decimal-clock-
Time is measured in duodecimal units. All other modern measurement such as length, volume etc are measured in decimal unit.For ease of calculation decimal system is considered better. The Decimal Clock — Prahar · Muhurat · Nimesha is a base-10 day system that reinterprets traditional Indian time names with modern.

## Proposed time unit
- 1 Muhurt = 100 Nimesha
- 1 Prahar = 100 Muhurut
- 1 Day = 10 Prahar

## Relation between proposed unit and current time unit
- 1 Nimesha = 0.864 second
- 1 Muhurt = 86.4 second
- 1 Prahar = 8640 second

## Components used to make digital decimal clock
- Arduino uno
- RTC (real time clock) DS1307
- LCD TFT Display ILI9341
- Jumper wires

## Libraries used
- RTClib
- Adafruit ILI9341
- Adafruit GFX Library

## Wiring (UNO)
**TFT (ILI9341 SPI)**
- CS → **D10**
- DC → **D8**
- RST → **D9**
- SCK → **D13**
- MOSI → **D11**
- MISO → **D12**
- VCC → **5V**
- GND → **GND**

**RTC (DS1307)**
- SDA → **A4**
- SCL → **A5**
- VCC → **5V**
- GND → **GND**

