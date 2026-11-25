// Zwei BNO085 ueber SPI, Ausgabe per WLAN und USB
// Zeile (26 Werte): tA1 a1 | tG1 g1 | tQ1 q1 | tA2 a2 | tG2 g2 | tQ2 q2
#include <Arduino.h>
#include <BNO08x_A.h> // Treiber-Kopie für Sensor 1
#include <BNO08x_B.h> // Treiber-Kopie für Sensor 2
#include <BNO08x_C.h> // Treiber-Kopie für Sensor 3
#include <BNO08x_D.h> // Treiber-Kopie für Sensor 4
#include <WiFi.h>
#include "freertos/ringbuf.h" // Ringpuffer zwischen den beiden Kernen (Kern 1 schreibt, Kern 0 liest)


// Pinbelegung: Chip-Select und Interrupt-Leitung je Sensor
#define CS_1 10
#define INT_1 1
#define CS_2 42
#define INT_2 21
#define CS_3 4
#define INT_3 41
#define CS_4 5
#define INT_4 39

BNO08x_A bno1(2);   // RST Sensor 1
BNO08x_B bno2(47);  // RST Sensor 2
BNO08x_C bno3(40);  // RST Sensor 3
BNO08x_D bno4(6);  // RST Sensor 4    

sh2_SensorValue_t w;  // Zwischenpuffer für ein empfangenes Sensor-Event 
float a[4][3], g[4][3], q[4][4]; //a=Beschl., g=Gyro, q=Quaternion, letzte Messwerte je Sensor speichern [Sensor][Achse]
unsigned long tA[4], tG[4], tQ[4];  // Zeitstempel der letzten Werte je Sensor speichern [Sensor]

WiFiServer server(5000);  // TCP-Server auf Port 5000 
WiFiClient client;        // aktuell verbundenen PC-Client (wird nur von Kern 0 benutzt)

RingbufHandle_t ring;   // Ringpuffer für fertige Zeilen (Kern 1 schreibt, Kern 0 liest)
volatile unsigned long verloren = 0;  // Anzahl der verlorenen Zeilen, wenn Ringpuffer voll war


// aktivieren der drei benötigten Reports eines Sensors
// Intervall 1000 mikro Sekunden = 100 Hz
// Delays geben dem Sensor Zeit, jeden Befehl zu verarbeiten.
template <class B> void reportsAn(B &b) {
  b.enableReport(SH2_LINEAR_ACCELERATION, 10000);  delay(100);
  b.enableReport(SH2_GYROSCOPE_CALIBRATED, 20000); delay(100);
  b.enableReport(SH2_GAME_ROTATION_VECTOR, 20000);
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

//Zeile fuer Sensor s an den Puffer anhaengen (Sensornummer 1-4 vorne)
void zeile(int s)
{
  char z[160];
  int n = snprintf(z, sizeof(z),
                  "%d | %lu %.4f %.4f %.4f | %lu %.4f %.4f %.4f | %lu %.4f %.4f %.4f %.4f\n",
                  s + 1, tA[s], a[s][0], a[s][1], a[s][2], tG[s], g[s][0], g[s][1], g[s][2],
                  tQ[s], q[s][0], q[s][1], q[s][2], q[s][3]);
  if(xRingbufferSend(ring, z, n, 0) != pdTRUE) verloren++;  // Puffer voll -> Zeilen zaehlen statt warten
}

// Sende-Task läuft dauerhaft auf Kern 0
// nimmt neue Laptops an und schickt alles aus dem Ringpuffer über Wlan
void sendeTask(void *)
{
  unsigned long tAus = 0;
  while (true)
  {
    // neuen Client annehmen, alte Verbindung dabei trennen
    WiFiClient neuer = server.available();
    if (neuer)
    {
      client.stop();
      client = neuer;
      client.setNoDelay(true);
    }

    // bis zu 1460 Byte aus dem Ringpuffer holen (wartet max 10 ms falls leer)
    size_t len;
    char * daten = (char *)xRingbufferReceiveUpTo(ring, &len, pdMS_TO_TICKS(10), 1460);
    if (daten)
    {
      if (client.connected())
      {
        client.write(daten, len); 
      }
      vRingbufferReturnItem(ring, daten); //Platz im Ringpuffer wieder freigeben
    }

    // einmal pro Sekunden melden, falls Zeit verloren wurde, weil Ringpuffer voll war
    if (millis() - tAus >= 1000)
    {
      if (verloren > 0)
      {
        Serial.printf("Ringpuffer voll, %lu Zeilen verloren\n", verloren);
        verloren = 0;
      }
      tAus = millis();
    }
  }
}

void setup() {
  // ESP32 spannt sein eigenes WLAN auf, der PC verbindet sich damit und kann die Daten empfangen
  WiFi.softAP("ESP32S3_BNO", "12345678"); //Passwort
  server.begin();

  Serial.begin(921600); // für USB hohe Baudrate, damit 100 Hz Zeilen sicher durchpassen
  
  ring = xRingbufferCreate(16384, RINGBUF_TYPE_BYTEBUF);  // 16 kBRingpuffer anlegen (Kern 1 schreibt, Kern 0 liest )

  // alle CS-Leitungen inaktiv (HIGH), damit sich die Sensoren beim STart nicht gegenseitig stören
  pinMode(CS_1, OUTPUT); digitalWrite(CS_1, HIGH);
  pinMode(CS_2, OUTPUT); digitalWrite(CS_2, HIGH);
  pinMode(CS_3, OUTPUT); digitalWrite(CS_3, HIGH);
  pinMode(CS_4, OUTPUT); digitalWrite(CS_4, HIGH);
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

  if (!bno3.begin_SPI(CS_3, INT_3)) 
    { 
      Serial.println("Sensor 3 nicht gefunden"); 
    }
    
  if (!bno4.begin_SPI(CS_4, INT_4)) 
    { 
      Serial.println("Sensor 4 nicht gefunden"); 
    }

    reportsAn(bno1); reportsAn(bno2); reportsAn(bno3); reportsAn(bno4);

    // Sende-Task auf Kern 0 starten (Funktion, Name, Stackgroesse, Parameter, Prioritaet, Handle, Kern)
    xTaskCreatePinnedToCore(sendeTask, "senden", 4096, NULL, 1, NULL, 0);
  }

void loop() {
  // Nach einem Sensor Reset sind alle Reports aus -> neu aktivieren 
  if (bno1.wasReset()) reportsAn(bno1);
  if (bno2.wasReset()) reportsAn(bno2);
  if (bno3.wasReset()) reportsAn(bno3);
  if (bno4.wasReset()) reportsAn(bno4);


  // INT = LOW bedeutet, dass ein Sensor neue Daten hat -> diese abholen und merken
  // NEU: bei neuer Beschleunigung sofort eine Zeile fuer diesen Sensor schreiben
  while (digitalRead(INT_1) == LOW && bno1.getSensorEvent(&w)) if (merke(0)) zeile(0);
  while (digitalRead(INT_2) == LOW && bno2.getSensorEvent(&w)) if (merke(1)) zeile(1);
  while (digitalRead(INT_3) == LOW && bno3.getSensorEvent(&w)) if (merke(2)) zeile(2);
  while (digitalRead(INT_4) == LOW && bno4.getSensorEvent(&w)) if (merke(3)) zeile(3);

}
