#include <Arduino.h>  // LIBRARY ARDUINO
#include <math.h> 
#define M_E 
#define ldrPin 4
const float gama = 0.7;
const float rl10 = 50

void setup() {
  Serial.begin(115200);
}

void loop() {
  delay(1000); 
  int nilaiLDR = analogRead(ldrPin);
  nilaiLDR = map(nilaiLDR, 4095, 0, 1024, 0); //mengubah nilai pembacaan sensor LDR dari nilai ADC arduino menjadi nilai ADC ESP32
  float voltase = nilaiLDR / 1024.*5;
  float resistansi = 2000 * voltase / (1-voltase/5);
  float ffa = abs((resistansi – 3472,6)/8024,2); //menerapkan persamaan regresi linier
  Serial.print("resistansi (kOhm) = ");
  Serial.println(resistansi);
  Serial.print("Asam lemak bebas = ");
  Serial.println(ffa);
}
