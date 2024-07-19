#include "HX711.h"
const int LOADCELL_DOUT_PIN = 18; // SENSOR LOAD CELL
const int LOADCELL_SCK_PIN = 5;  // SENSOR LOAD CELL

HX711 scale;

float calibration_factor = 230.6059; //Hasil Kalibrasi
float units;

void setup()
{
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare();  //Mengembalikan skala ke 0 

  long zero_factor = scale.read_average(); //pembacaan dasar
  Serial.print("Zero factor: "); // Digunakan untuk menghilangkan kebutuhan terhadap tare scale. 
  Serial.println(zero_factor);
}

void loop()
{
  scale.set_scale(calibration_factor); // Sesuaikan dengan faktor kalibrasi

  Serial.print("Pengukuran ");
  units = scale.get_units(), 1;
  if (units < 0)
  {
    units = 0.00;
  }
  Serial.print("Massa: ");
  Serial.print(units);
  Serial.print(" gram");
  Serial.println();
  delay(240000); //delay 240s
}
