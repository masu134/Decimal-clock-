#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// ==== TFT pins (UNO + hardware SPI) ====
#define TFT_CS   10
#define TFT_DC    8
#define TFT_RST   9

// ==== Objects ====
RTC_DS1307 rtc;
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

// ==== Clock layout ====
const int centerX = 120;
const int centerY = 120;
const int radiusOuter = 100;

// Labels at the 5 major positions (clockwise from top)
const uint8_t PRAHAR_LABELS[5] = {1, 2, 3, 4, 5};

// Previous hand endpoints (for erasing)
int prevNimX = 0, prevNimY = 0;
int prevMuhX = 0, prevMuhY = 0;
int prevPrahX = 0, prevPrahY = 0;

// ==== Forward decls ====
void drawClockFace();
void drawClockHands(int prahar, int muhurat, int nimesha);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1) {}
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC not running – setting compile time.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextWrap(false);

  drawClockFace();
}

void loop() {
  DateTime now = rtc.now();

  // Seconds since midnight
  long totalSec = now.hour() * 3600L + now.minute() * 60L + now.second();

  // Decimal time (1 nimesha = 0.864 s)
  const float nimeshaLen = 0.864f;
  long totalNimesha = (long)(totalSec / nimeshaLen);

  int prahar  = totalNimesha / 10000;           // 0..9
  int muhurat = (totalNimesha / 100) % 100;     // 0..99
  int nimesha =  totalNimesha % 100;            // 0..99

  Serial.print("Prahar: ");  Serial.print(prahar);
  Serial.print(" | Muhurat: "); Serial.print(muhurat);
  Serial.print(" | Nimesha: "); Serial.println(nimesha);

  drawClockHands(prahar, muhurat, nimesha);
  delay(1000);
}

// ====== Draw static dial ======
void drawClockFace() {
  tft.fillScreen(ILI9341_BLACK);

  // Outer circle
  tft.drawCircle(centerX, centerY, radiusOuter, ILI9341_WHITE);

  // Major labels 1..5
  for (int i = 0; i < 5; i++) {
    float ang = radians(i * 72.0f - 90.0f);   // start at top, clockwise
    int r = 75;
    int x = centerX + (int)(cos(ang) * r) - 5;
    int y = centerY + (int)(sin(ang) * r) - 5;

    tft.setCursor(x, y);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.print(PRAHAR_LABELS[i]);
  }

  // 50 ticks (every 7.2°), longer every 10th
  for (int i = 0; i < 50; i++) {
    float ang = radians(i * 7.2f - 90.0f);
    int r1 = (i % 10 == 0) ? 90 : 95;
    int r2 = radiusOuter;

    int x1 = centerX + (int)(cos(ang) * r1);
    int y1 = centerY + (int)(sin(ang) * r1);
    int x2 = centerX + (int)(cos(ang) * r2);
    int y2 = centerY + (int)(sin(ang) * r2);

    tft.drawLine(x1, y1, x2, y2, ILI9341_WHITE);
  }
}

// ====== Draw / update hands ======
void drawClockHands(int p, int m, int n) {
  // Erase previous hands
  if (prevNimX || prevNimY)   tft.drawLine(centerX, centerY, prevNimX,  prevNimY,  ILI9341_BLACK);
  if (prevMuhX || prevMuhY)   tft.drawLine(centerX, centerY, prevMuhX,  prevMuhY,  ILI9341_BLACK);
  if (prevPrahX || prevPrahY) tft.drawLine(centerX, centerY, prevPrahX, prevPrahY, ILI9341_BLACK);

  // Nimesha: 0..99 → 0..360
  float angleNim = (n) * 3.6f;

  // Muhurat: include nimesha fraction for smooth sweep
  float angleMuh = (m + n / 100.0f) * 3.6f;

  // Prahar with 5→1 rollover fix for the second half of the day
  // Map prahar 0..4 to sectors 1..5, and prahar 5..9 to sectors starting at "5"
  float phase = (p % 5) + (m + n / 100.0f) / 100.0f; // 0..5 position within 5 sectors
  if (p >= 5) phase += 4.0f;                         // shift second cycle to start at label "5"
  if (phase >= 5.0f) phase -= 5.0f;
  float anglePrah = phase * 72.0f;

  // Draw nimesha (red)
  float a = radians(angleNim - 90.0f);
  prevNimX = centerX + (int)(cos(a) * 90);
  prevNimY = centerY + (int)(sin(a) * 90);
  tft.drawLine(centerX, centerY, prevNimX, prevNimY, ILI9341_RED);

  // Draw muhurat (blue)
  a = radians(angleMuh - 90.0f);
  prevMuhX = centerX + (int)(cos(a) * 70);
  prevMuhY = centerY + (int)(sin(a) * 70);
  tft.drawLine(centerX, centerY, prevMuhX, prevMuhY, ILI9341_BLUE);

  // Draw prahar (green)
  a = radians(anglePrah - 90.0f);
  prevPrahX = centerX + (int)(cos(a) * 50);
  prevPrahY = centerY + (int)(sin(a) * 50);
  tft.drawLine(centerX, centerY, prevPrahX, prevPrahY, ILI9341_GREEN);
}