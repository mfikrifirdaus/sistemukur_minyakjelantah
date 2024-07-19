#include <Arduino.h> // LIBRARY ARDUINO
#include <math.h>
#include <SimpleTimer.h>

// COMMAND PROGRAM HEADER, VALUE SET, AND PIN 
SimpleTimer timer;
#define M_E
#define Reed 34   // SENSOR MAGNET REED 1
#define Reed1 15  // SENSOR MAGNET REED 2
float waktu;
unsigned long long waktumillis; //waktu milis arduino
unsigned long long waktuawal;  //waktu awal
unsigned long long waktuakhir; //waktu akhir
float waktuttempuh;   //selisih waktu akhir dengan awal
int reedup;           //sensor magnetic reed atas
int reedbottom;       //sensor magnetic reed bawah

// COMMAND PROGRAM SETUP
void setup() {
  pinMode(Reed, INPUT);
  pinMode(Reed1, INPUT);
  Serial.begin(115200);
}

// COMMAND PROGRAM MODE PENGUKURAN
void pengukuranwaktu() {
    reedup = digitalRead(Reed);
    reedbottom = digitalRead(Reed1);
    if  (reedup == 0) {
      waktuawal = waktumillis;
      Serial.print("Waktu Awal = ");
      Serial.print(waktuawal);
      Serial.println("========================== ");
    }
    if (reedbottom == 0) {
      waktuakhir = waktumillis;
      Serial.print("Waktu akhir = ");
      Serial.println(waktuakhir);
      waktuttempuh = (waktuakhir - waktuawal);
      Serial.println(waktuakhir);
      Serial.print("Waktu tempuh = ");
      Serial.println(waktuttempuh);
    }
  }

void loop() {
  waktumillis = millis();
}
