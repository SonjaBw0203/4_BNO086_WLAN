// Zwei BNO085 ueber SPI, Ausgabe per WLAN und USB
// Zeile (26 Werte): tA1 a1 | tG1 g1 | tQ1 q1 | tA2 a2 | tG2 g2 | tQ2 q2
#include <Arduino.h>
#include <BNO08x_A.h> // Treiber-Kopie für Sensor 1
#include <BNO08x_B.h> // Treiber-Kopie für Sensor 2
#include <WiFi.h>

// Pinbelegung: Chip-Select und Interrupt-Leitung je Sensor
#define CS_1 10
#define INT_1 1
#define CS_2 42
#define INT_2 21

BNO08x_A bno1(2);   // RST Sensor 1
BNO08x_B bno2(47);  // RST Sensor 2

sh2_SensorValue_t w;  // Zwischenpuffer für ein empfangenes Sensor-Event 
float a[2][3], g[2][3], q[2][4]; //a=Beschl., g=Gyro, q=Quaternion, letzte Messwerte je Sensor speichern [Sensor][Achse]
unsigned long tA[2], tG[2], tQ[2];  // Zeitstempel der letzten Werte je Sensor speichern [Sensor]

WiFiServer server(5000);  // TCP-Server auf Port 5000 
WiFiClient client;        // aktuell verbundenen PC-Client 

// aktivieren der drei benötigten Reports eines Sensors
// Intervall 1000 mikro Sekunden = 100 Hz
// Delays geben dem Sensor Zeit, jeden Befehl zu verarbeiten.
template <class B> void reportsAn(B &b) {
  b.enableReport(SH2_LINEAR_ACCELERATION, 10000);  delay(100);
  b.enableReport(SH2_GYROSCOPE_CALIBRATED, 10000); delay(100);
  b.enableReport(SH2_GAME_ROTATION_VECTOR, 10000);
}

// Sortieren den empfangenen Datenpakets (w) an die richtige Stelle:
// w.sensorId gibt an, um welchen Sensorwert es sich handelt (Beschl., Gyro, Quaternion)
// dann in a, g oder q speichern, Zeitstempel in tA, tG oder tQ
// s gibt an , von welchem Sensor die Daten stammen (0 = bno1 oder 1 = bno2)
// Rückgabe true nur bei neuer Beschleunigung -> löst eine Ausgabezeile aus
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
  // ESP32 spannt sein eigenes WLAN auf, der PC verbindet sich damit und kann die Daten empfangen
  WiFi.softAP("ESP32S3_BNO", "12345678"); //Passwort
  server.begin();

  Serial.begin(921600); // für USB hohe Baudrate, damit 100 Hz Zeilen sicher durchpassen
  
  // Beide CS-Leitungen inaktiv (HIGH), damit sich die Sensoren beim STart nicht gegenseitig stören
  pinMode(CS_1, OUTPUT); digitalWrite(CS_1, HIGH);
  pinMode(CS_2, OUTPUT); digitalWrite(CS_2, HIGH);
  delay(2000);

  // Beide Sensoren über SPI initialisieren, falls einer nicht gefunden wird, Programm anhalten
  if (!bno1.begin_SPI(CS_1, INT_1)) 
    { 
      Serial.println("Sensor 1 nicht gefunden"); 
    }

  if (!bno2.begin_SPI(CS_2, INT_2)) 
    {
    Serial.println("Sensor 2 nicht gefunden");
    } 
  }

void loop() {
  // neuen Client annehmen, falls noch keiner verbunden ist
  if (!client.connected()) 
  { client = server.available(); 
  if (client) client.setNoDelay(true); }

  // Nach einem Sensor Reset sind alle Reports aus -> neu aktivieren 
  if (bno1.wasReset()) reportsAn(bno1);
  if (bno2.wasReset()) reportsAn(bno2);

  // INT = LOW bedeutet, dass ein Sensor neue Daten hat -> diese abholen und merken
  bool neu = false;
  if (digitalRead(INT_1) == LOW && bno1.getSensorEvent(&w)) neu = merke(0);
  if (digitalRead(INT_2) == LOW && bno2.getSensorEvent(&w)) merke(1);
  if (!neu) return;

  // Ausgabenzeile zusammenbauen: für jeden Sensor die jeweils letzten Werten
  // n zählt die bisher geschriebenen Zeichen 
  char z[256];
  int n = 0;
  for (int s = 0; s < 2; s++)
    n += snprintf(z + n, sizeof(z) - n, "%s%lu %.4f %.4f %.4f | %lu %.4f %.4f %.4f | %lu %.4f %.4f %.4f %.4f",
                  s ? " | " : "", tA[s], a[s][0], a[s][1], a[s][2], tG[s], g[s][0], g[s][1], g[s][2],
                  tQ[s], q[s][0], q[s][1], q[s][2], q[s][3]);
  z[n++] = '\n';  // zeilenende

  Serial.write(z, n);   // Ausgabe über USB
 
    if (client.connected()) client.write(z, n); // Ausgabe über WLAN, falls Client da 
}