// Test: einen BNO085 ueber SPI auslesen (zweiter Sensor haengt mit am Bus)
// Ausgabe: t_us | ax ay az | gx gy gz | qr qi qj qk

#include <Arduino.h>
#include <Adafruit_BNO08x.h>

// Sensor 1: CS 10, INT 14, RST 15   |   Sensor 2: CS 9, INT 16, RST 17
#define BNO_CS    9
#define BNO_INT   16
#define BNO_RST   17

Adafruit_BNO08x   bno(BNO_RST);
sh2_SensorValue_t w;

float a[3], g[3], q[4] = {1, 0, 0, 0};

void reportsAn() {
  bno.enableReport(SH2_LINEAR_ACCELERATION, 10000);    // 100 Hz
  delay(100);
  bno.enableReport(SH2_GYROSCOPE_CALIBRATED, 10000);
  delay(100);
  bno.enableReport(SH2_GAME_ROTATION_VECTOR, 10000);
}

void setup() {
  Serial.begin(921600);
  pinMode(9, OUTPUT);  digitalWrite(9, HIGH);    // CS Sensor 2 inaktiv
  pinMode(10, OUTPUT); digitalWrite(10, HIGH);   // CS Sensor 1 inaktiv
  delay(2000);
  if (!bno.begin_SPI(BNO_CS, BNO_INT)) {
    Serial.println("BNO085 nicht gefunden");
    while (true) delay(1000);
  }
  Serial.println("BNO085 bereit");
  reportsAn();
}

void loop() {
  if (bno.wasReset()) reportsAn();
  if (!bno.getSensorEvent(&w)) return;

  switch (w.sensorId) {
    case SH2_GYROSCOPE_CALIBRATED:
      g[0] = w.un.gyroscope.x; g[1] = w.un.gyroscope.y; g[2] = w.un.gyroscope.z;
      break;
    case SH2_GAME_ROTATION_VECTOR:
      q[0] = w.un.gameRotationVector.real; q[1] = w.un.gameRotationVector.i;
      q[2] = w.un.gameRotationVector.j;    q[3] = w.un.gameRotationVector.k;
      break;
    case SH2_LINEAR_ACCELERATION:
      a[0] = w.un.linearAcceleration.x; a[1] = w.un.linearAcceleration.y;
      a[2] = w.un.linearAcceleration.z;
      Serial.printf("%lu | %.4f %.4f %.4f | %.4f %.4f %.4f | %.4f %.4f %.4f %.4f\n",
                    (unsigned long)micros(), a[0], a[1], a[2], g[0], g[1], g[2],
                    q[0], q[1], q[2], q[3]);
      break;
  }
}