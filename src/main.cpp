// Zwei BNO085 ueber SPI, Ausgabe per WLAN und USB
// Zeile (26 Werte): tA1 a1 | tG1 g1 | tQ1 q1 | tA2 a2 | tG2 g2 | tQ2 q2
#include <Arduino.h>
#include <BNO08x_A.h>
#include <BNO08x_B.h>
#include <WiFi.h>

#define CS_1 10
#define INT_1 14
#define CS_2 9
#define INT_2 16

BNO08x_A bno1(15);   // RST_1
BNO08x_B bno2(17);   // RST_2
sh2_SensorValue_t w;
float a[2][3], g[2][3], q[2][4];
unsigned long tA[2], tG[2], tQ[2];

WiFiServer server(5000);
WiFiClient client;


template <class B> void reportsAn(B &b) {
  b.enableReport(SH2_LINEAR_ACCELERATION, 10000);  delay(100);
  b.enableReport(SH2_GYROSCOPE_CALIBRATED, 10000); delay(100);
  b.enableReport(SH2_GAME_ROTATION_VECTOR, 10000);
}

// Wert von Sensor s speichern; true bei neuer Beschleunigung
bool merke(int s) {
  unsigned long t = micros();
  auto &u = w.un;
  switch (w.sensorId) {
    case SH2_LINEAR_ACCELERATION:
      tA[s] = t; a[s][0] = u.linearAcceleration.x; a[s][1] = u.linearAcceleration.y; a[s][2] = u.linearAcceleration.z;
      return true;
    case SH2_GYROSCOPE_CALIBRATED:
      tG[s] = t; g[s][0] = u.gyroscope.x; g[s][1] = u.gyroscope.y; g[s][2] = u.gyroscope.z;
      break;
    case SH2_GAME_ROTATION_VECTOR:
      tQ[s] = t; q[s][0] = u.gameRotationVector.real; q[s][1] = u.gameRotationVector.i;
      q[s][2] = u.gameRotationVector.j; q[s][3] = u.gameRotationVector.k;
      break;
  }
  return false;
}

void setup() {
  WiFi.softAP("ESP32S3_BNO", "12345678"); //Passwort
  server.begin();

  Serial.begin(921600);
  pinMode(CS_1, OUTPUT); digitalWrite(CS_1, HIGH);
  pinMode(CS_2, OUTPUT); digitalWrite(CS_2, HIGH);
  delay(2000);
  if (!bno1.begin_SPI(CS_1, INT_1) || !bno2.begin_SPI(CS_2, INT_2)) {
    Serial.println("Sensor nicht gefunden"); while (true) delay(1000);
  }
  reportsAn(bno1); reportsAn(bno2);

}

void loop() {
  if (!client.connected()) 
  { client = server.available(); 
  if (client) client.setNoDelay(true); }
  if (bno1.wasReset()) reportsAn(bno1);
  if (bno2.wasReset()) reportsAn(bno2);

  bool neu = false;
  if (digitalRead(INT_1) == LOW && bno1.getSensorEvent(&w)) neu = merke(0);
  if (digitalRead(INT_2) == LOW && bno2.getSensorEvent(&w)) merke(1);
  if (!neu) return;

  char z[256];
  int n = 0;
  for (int s = 0; s < 2; s++)
    n += snprintf(z + n, sizeof(z) - n, "%s%lu %.4f %.4f %.4f | %lu %.4f %.4f %.4f | %lu %.4f %.4f %.4f %.4f",
                  s ? " | " : "", tA[s], a[s][0], a[s][1], a[s][2], tG[s], g[s][0], g[s][1], g[s][2],
                  tQ[s], q[s][0], q[s][1], q[s][2], q[s][3]);
  z[n++] = '\n';

  Serial.write(z, n);
 

    if (client.connected()) client.write(z, n);
}