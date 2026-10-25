 // Zwei BNO085 ueber SPI auslesen, eine Zeile pro Messzeitpunkt (100 Hz):
// tA1 ax1 ay1 az1 | tG1 gx1 gy1 gz1 | tQ1 qr1 qi1 qj1 qk1 | tA2 ax2 ay2 az2 | tG2 gx2 gy2 gz2 | tQ2 qr2 qi2 qj2 qk2
#include <Arduino.h>
#include <BNO08x_A.h>
#include <BNO08x_B.h>


// Pins für beide Sensoren
// Sensor 1: CS 10, INT 14, RST 15   |   Sensor 2: CS 9, INT 16, RST 17
#define CS_1    10
#define INT_1   14
#define RST_1   15
#define CS_2     9
#define INT_2   16
#define RST_2   17

//zwei Objekte für die beiden Sensoren
BNO08x_A          bno1(RST_1);   
BNO08x_B          bno2(RST_2);
sh2_SensorValue_t w;

// Werte je Sensor, [0] = Sensor 1, [1] = Sensor 2
float a[2][3], g[2][3], q[2][4] = {{1, 0, 0, 0}, {1, 0, 0, 0}};

// Variable für die Zeitstempel der Sensoren, um die Zeitdifferenz zu berechnen
unsigned long tA[2], tG[2], tQ[2]; // Zeitpunkte Beschleunigung, Drehrate/Gyroskop, Quaternion


// für Sensor 1
void reportsAn1() {
  bno1.enableReport(SH2_LINEAR_ACCELERATION, 10000);    // 100 Hz
  delay(100);
  bno1.enableReport(SH2_GYROSCOPE_CALIBRATED, 10000);
  delay(100);
  bno1.enableReport(SH2_GAME_ROTATION_VECTOR, 10000);
}

// für Sensor 2
void reportsAn2() {
  bno2.enableReport(SH2_LINEAR_ACCELERATION, 10000);    // 100 Hz
  delay(100);
  bno2.enableReport(SH2_GYROSCOPE_CALIBRATED, 10000);
  delay(100);
  bno2.enableReport(SH2_GAME_ROTATION_VECTOR, 10000);
}

// switch gibt true zurück, wenn eine neue Beschleunigung kam 
bool merke(int s) {
switch (w.sensorId) {
    case SH2_GYROSCOPE_CALIBRATED:
      g[s][0] = w.un.gyroscope.x; g[s][1] = w.un.gyroscope.y; g[s][2] = w.un.gyroscope.z;
      tG[s] = micros();
      break;
    case SH2_GAME_ROTATION_VECTOR:
      q[s][0] = w.un.gameRotationVector.real; q[s][1] = w.un.gameRotationVector.i;
      q[s][2] = w.un.gameRotationVector.j;    q[s][3] = w.un.gameRotationVector.k;

      tQ[s] = micros();
      break;
    case SH2_LINEAR_ACCELERATION:
      a[s][0] = w.un.linearAcceleration.x; a[s][1] = w.un.linearAcceleration.y;
      a[s][2] = w.un.linearAcceleration.z;

      tA[s] = micros();
        return true;    
  }
  return false;
}

void setup() {
  Serial.begin(921600);
  pinMode(CS_2, OUTPUT);  digitalWrite(CS_2, HIGH);    // CS Sensor 2 inaktiv
  pinMode(CS_1, OUTPUT); digitalWrite(CS_1, HIGH);   // CS Sensor 1 inaktiv
  delay(2000);
    if (!bno1.begin_SPI(CS_1, INT_1)) {
    Serial.println("Sensor 1 nicht gefunden");
    while (true) delay(1000);
  }
  if (!bno2.begin_SPI(CS_2, INT_2)) {                // NEU
    Serial.println("Sensor 2 nicht gefunden");
    while (true) delay(1000);
  }
  Serial.println("Beide BNO085 bereit");
  reportsAn1();
  reportsAn2();
}

void loop() {
  if (bno1.wasReset()) reportsAn1();
  if (bno2.wasReset()) reportsAn2();

  // Sensor nur abfragen, wenn sein INT-Pin LOW ist (=neue Daten)
  // Sonst wartet die Bibliothek bis zu 500 ms und blockiert den anderen Sensor
  bool neueZeile = false;
  if (digitalRead(INT_1) == LOW && bno1.getSensorEvent(&w)) neueZeile = merke(0);
  if (digitalRead(INT_2) == LOW && bno2.getSensorEvent(&w)) merke(1); 

    // gemeinsame Zeile, sobald Sensor 1 eine neue Beschleunigung liefert
  if (neueZeile) {
    Serial.printf("%lu %.4f %.4f %.4f | %lu %.4f %.4f %.4f | %lu %.4f %.4f %.4f %.4f"
                  " | %lu %.4f %.4f %.4f | %lu %.4f %.4f %.4f | %lu %.4f %.4f %.4f %.4f\n",
                  (unsigned long)micros(),
                  tA[0], a[0][0], a[0][1], a[0][2],
                  tG[0], g[0][0], g[0][1], g[0][2],
                  tQ[0], q[0][0], q[0][1], q[0][2], q[0][3],
                  tA[1], a[1][0], a[1][1], a[1][2],
                  tG[1], g[1][0], g[1][1], g[1][2],
                  tQ[1], q[1][0], q[1][1], q[1][2], q[1][3]);
  }  
}