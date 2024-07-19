//----------- COMMAND LIBRARY -----------
#include <Arduino.h> // LIBRARY ARDUINO
#include <HX711.h>
#include "HX711.h"
#include "soc/rtc.h"
#include <LiquidCrystal_I2C.h> // LIBRARY LCD
#include <math.h>
#include <SimpleTimer.h>

//----------- COMMAND PROGRAM HEADER, VALUE SET, AND PIN -----------
SimpleTimer timer;
#define M_E
#define Reed1 34  // Sensor Magnetic Reed 1
#define Reed2 15  // Sensor Magnetic Reed 2
#define ldrPin 4  // SENSOR LDR
byte buttondensitas = 35;
int m1 = 0;  //Mode Pengujian Densitas
int m2;  //Mode Pengujian Viskositas
int m3;  //Mode Pengujian FFA
int b_densitas;  // Untuk Simpan Nilai Button Densitas
const float gama = 0.7;  // Setting Nilai Sensor LDR
const float rl10 = 50;  // Setting Nilai Sensor LDR
const int relay = 2;  // Modul Laser
const int LOADCELL_DOUT_PIN = 18;  // Sensor Load Cell
const int LOADCELL_SCK_PIN = 5;  // Sensor Load Cell
HX711 scale;  // Sensor Load Cell
int lcdColumns = 20;  // Kolom LCD 20x4
int lcdRows = 4;  // Baris LCD 20x4
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  // Alamat LCD 20x4
float m_minyak;  // Massa minyak jelantah
float v_minyak = 0.00010 ;  // volume minyak dalam satuan m^3 (sesuaikan dengan di lab yakni 100ml)
float m_tabung = 0.1133083;  // massa tabung kosong dalam satuan kg
float densitas_minyak;
float r_bolamagnet = 0.0049195;  // jari-jari bola magnet dalam satuan m
float massa_bolamagnet = 0.0037847;  // massa bola magnet dalam satuan kg
float volume_bolamagnet = ((4 / 3) * 3.1428571428571428571428571428571 * r_bolamagnet*r_bolamagnet*r_bolamagnet); // volume bola dalam satuan m^3 
float densitas_bolamagnet = (massa_bolamagnet/volume_bolamagnet); // rumus densitas suatu benda
float g_bumi = 9.8; // percepatan gravitasi bumi dalam satuan m/(s^2)
float jarak_sensormagnet = 0.109; // jarak sensor magnetic reed atas hingga sensor magnetic reed bawah dalam satuan meter
float waktu;
float nilai_viskositasminyak; 
unsigned long long waktumillis;  // waktu milis arduino
unsigned long long waktuawal;  // waktu awal ketika medan magnet dari bola magnet mengenai sensor magnetic reed bagian atas
unsigned long long waktuakhir; // waktu akhir ketika medan magnet dari bola magnet mengenai sensor magnetic reed bagian bawah
float waktutempuh;  // selisih waktu akhir dengan awal
float speeds;  //kecepatan dengan satuan m/s, rumus (v = s/t)
int magneticreed_up;   //sensor magnetic reed yang dihubungkan secara paralel di bagian atas
int magneticreed_bottom;  //sensor magnetic reed yang dihubungkan secara paralel di bagian bawah
float viscosity;  //viskositas
float AsamLemakBebas;
float FFA;

//----------- COMMAND MILLIS -----------
unsigned long prevTime_T1 = millis(); // sensor load cell
unsigned long prevTime_T2 = millis(); // sensor PB LOCK densitas
unsigned long prevTime_T3 = millis(); // sensor LDR
unsigned long prevTime_T4 = millis(); // LCD
unsigned long prevTime_T5 = millis(); // sensor PB LOCK viskositas
unsigned long prevTime_T6 = millis(); // sensor PB LOCK asam lemak bebas

long interval_T1 = 100; // jumlah waktu yang harus dipenuhi agar sensor magnetic reed bisa membaca ulang (dalam waktu 0,1 detik)  
long interval_T2 = 5000; // jumlah waktu yang harus dipenuhi agar sensor load cell bisa membaca ulang (dalam waktu 5 detik)
long interval_T3 = 5000; // jumlah waktu yang harus dipenuhi agar sensor LDR bisa membaca ulang (dalam waktu 5 detik)
long interval_T4 = 5000; // jumlah waktu yang harus dipenuhi agar program benar-benar selesai (dalam waktu 5 detik)

//----------- COMMAND PROGRAM SETUP -----------
void setup() {
  pinMode(Reed1, INPUT);
  pinMode(Reed2, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(buttondensitas, INPUT_PULLUP);
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  // mencetak rata-rata 20 pembacaan dari ADC
  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));  // mencetak rata-rata 5 pembacaan dari ADC dikurangi tare weight (belum diset)
  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // cetak rata-rata 5 pembacaan dari ADC dikurangi tare weight (tidak diset) dibagi dengan parameter SCALE (belum disetel)
  scale.set_scale(230.6059);  //  faktor kalibrasi, merupakan hasil pembagian pada pembacaan sensor load cell dibagi dengan massa benda yang diketahui
  scale.tare();  // atur ulang skala ke 0
  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  // mencetak rata-rata 20 pembacaan dari ADC
  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));  // cetak rata-rata 5 pembacaan dari ADC dikurangi tare weight, atur dengan tare()
  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // cetak rata-rata 5 pembacaan dari ADC dikurangi tare weight, dibagi dengan parameter SCALE yang diatur dengan set_scale
  lcd.init();
  lcd.backlight();
}

void modepengujian () {
  m1 = digitalRead(buttondensitas);
  if (m1 == 1) {
    m2 = m2 + 1;
  }
  if (m2 > 30000) {
    m3 = m3 + 1;
    Serial.println(m3);
    if (m3 == 0) {
      lcd.setCursor(6, 0);
      lcd.print("Alat Aktif");
    }
    else if (m3 == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Mode Densitas");
      lcd.setCursor(0, 1);
      lcd.print("Dsts=          kg/m3");
    }
    else if (m3 == 2) {
      lcd.setCursor(0, 0);
      lcd.print("Mode Viskositas    ");
      lcd.setCursor(0, 1);
      lcd.print("Dsts=          kg/m3");
      lcd.setCursor(0, 2);
      lcd.print("Vsts=        mPa.s");
    }
    else if (m3 == 3) {
      lcd.setCursor(0, 0);
      lcd.print("Mode FFA          ");
      lcd.setCursor(0, 1);
      lcd.print("Dsts=          kg/m3");
      lcd.setCursor(0, 2);
      lcd.print("Vsts=        mPa.s");
      lcd.setCursor(0, 3);
      lcd.print("FFA =        %");
    }
    else if (m3 == 4) {
      lcd.setCursor(0, 0);
      lcd.print("Pengukuran Selesai");
      lcd.setCursor(0, 1);
      lcd.print("Dsts=          kg/m3");
      lcd.setCursor(0, 2);
      lcd.print("Vsts=        mPa.s");
      lcd.setCursor(0, 3);
      lcd.print("FFA =        %");
    }
    else if (m3 == 5) {
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Selesai");
    }
    m2 = 0;
  }
  if (m3 > 4) {

    m3 = 0;

}

//--------------------COMMAND PROGRAM MODE PENGUKURAN-------------------
void pengukuran_ketigaparameter() {
  //----------------------Pengukuran DENSITAS-------------------------
  if (m3 == 1) {
    unsigned long currentTime2 = millis();
    if ((currentTime2 - prevTime_T2) > interval_T2) {
      m_minyak = (scale.get_units() / 1000);  // mengubah satuan massa minyak dari gram ke kilogram 
      densitas_minyak = ((m_minyak - m_tabung) / v_minyak);  // mengurangi massa minyak dengan massa gelas ukur, satuan densitas adalah kg/m^3

      Serial.print("Massa Minyak & Densitas Minyak = ");  // Catatan : Tidak perlu menambahkan else, karena jika bernilai 0 sudah otomatis (lanjut kebawah)
      Serial.print (m_minyak);  // menghentikan pembacaan nilai densitas, dan nilai densitas sebelumnya sudah tercatat setelah Pushbutton dilepas
      Serial.print(" & ");
      Serial.println(densitas_minyak);
      lcd.setCursor(6, 1);  // dipakai untuk menampilkan angka di LCD
      lcd.print(densitas_minyak);  // dipakai untuk menampilkan angka di LCD
      prevTime_T2 = millis();
    }
  }
  //----------------------Pengukuran VISKOSITAS-------------------------
  if (m3 == 2) {
    magneticreed_up = digitalRead(Reed1);
    magneticreed_bottom = digitalRead(Reed2);
    if  (magneticreed_up == 0) {
      waktuawal = waktumillis;
      Serial.print("Waktu Awal = ");
      Serial.print(waktuawal);
      Serial.println("========================== ");
    }
    if (magneticreed_bottom == 0) {
      waktuakhir = waktumillis;
      Serial.print("Waktu awal di dalem redbottom= ");
      Serial.println(waktuakhir);
      waktutempuh = (waktuakhir - waktuawal);
      speeds = jarak_sensormagnet / (waktutempuh / 1000); // waktu tempuh dibagi 1000 supaya satuannya menjadi detik bukan milisecond, sehingga satuan kecepatan menjadi m/s
      viscosity = (2 * r_bolamagnet * r_bolamagnet * g_bumi * (densitas_bolamagnet - densitas_minyak)) / (9 * speeds);  // rumus mendapatkan viskositas menggunakan hukum Stokes (metode bola jatuh)

      Serial.println(waktuakhir);
      Serial.print("Waktu tempuh = ");
      Serial.println(waktutempuh);
      Serial.println("========================== ");
      Serial.print("Kecepatan = ");
      Serial.print(speeds);
      Serial.println("  m/s");
      float nilai_viskositasminyak = viscosity;
      Serial.print("Viskositas = ");
      Serial.print(nilai_viskositasminyak);  // Nilai VISKOSITAS 
      Serial.println("mPa.s");
      lcd.setCursor(6, 1);  // berfungsi untuk menampilkan angka di LCD
      lcd.print(densitas_minyak);
      lcd.setCursor(6, 2);  // berfungsi untuk menampilkan angka di LCD
      lcd.print(nilai_viskositasminyak);  // berfungsi untuk menampilkan angka di LCD
      delay(5000); // Sebelum ada Keterangan "OK", Bola Magnet Harus Sudah Diangkat Keluar dari Tabung Selama 5 Detik Agar Nilai Viskositas Tidak Berubah
      lcd.setCursor(18, 0);
      lcd.print("OK");
    }

    unsigned long currentTime1 = millis();
    if ((currentTime1 - prevTime_T1) > interval_T1) {
      lcd.setCursor(6, 1);  // berfungsi untuk menampilkan angka di LCD
      lcd.print(densitas_minyak);  // berfungsi untuk menampilkan angka di LCD
      nilai_viskositasminyak;
      lcd.setCursor(6, 2);  // berfungsi untuk menampilkan angka di LCD
      lcd.print(nilai_viskositasminyak);  // berfungsi untuk menampilkan angka di LCD
      prevTime_T1 = millis();
    }
  }

  //----------------------Pengukuran FFA -------------------------
  if (m3 == 3) {
    unsigned long currentTime3 = millis();
    if ((currentTime3 - prevTime_T3) > interval_T3) {
      int nilaiLDR = analogRead(ldrPin);
      Serial.println(nilaiLDR);
      nilaiLDR = map(nilaiLDR, 4095, 0, 1024, 0);  // mengubah nilai pembacaan sensor LDR dari nilai ADC arduino menjadi nilai ADC ESP32
      float voltase2 = nilaiLDR / 1024.*5;
      float resistansi2 = 2000 * voltase2 / (1 - voltase2 /5 );  // satuan resistansi adalah kilo ohm
      AsamLemakBebas =(resistansi2 – 3722,8)/7952,1;  // menerapkan persamaan regresi linier

      Serial.print("FFA = ");  // Catatan : Tidak perlu menambahkan else, karena jika bernilai 0 sudah otomatis (lanjut kebawah)
      Serial.println (AsamLemakBebas);
      // menghentikan pembacaan nilai densitas, dan nilai densitas sebelumnya sudah tercatat setelah Pushbutton dilepas
      lcd.setCursor(6, 1);  // berfungsi untuk menampilkan angka di LCD
      lcd.print(densitas_minyak);  // berfungsi untuk menampilkan angka di LCD

      lcd.setCursor(6, 2);  // berfungsi untuk menampilkan angka di LCD
      lcd.print(nilai_viskositasminyak);  // berfungsi untuk menampilkan nilai viskositas di LCD

      lcd.setCursor(6, 3); // berfungsi untuk menampilkan angka di LCD
      lcd.print(AsamLemakBebas, 4);
      prevTime_T3 = millis();
    }
  }

  if (m3 == 4) {
    unsigned long currentTime4 = millis();
    if ((currentTime4 - prevTime_T4) > interval_T4) {
      lcd.setCursor(6, 1);  // berfungsi untuk menampilkan angka di LCD
      lcd.print(densitas_minyak);  // berfungsi untuk menampilkan angka di LCD

      lcd.setCursor(6, 2);  // berfungsi untuk menampilkan angka di LCD
      lcd.print(nilai_viskositasminyak);  // berfungsi untuk menampilkan nilai viskositas di LCD

      lcd.setCursor(6, 3); // berfungsi untuk menampilkan angka di LCD
      lcd.print(AsamLemakBebas, 4);
      prevTime_T4 = millis();
    }
  }
}

//----------- COMMAND PROGRAM MODUL LASER -----------
void modul_laser() {
  digitalWrite(relay, LOW);
}

//----------- KUMPULAN PENGATURAN BERULANG -----------
void loop() {
  waktumillis = millis();
  modul_laser();
  modepengujian();
  pengukuran_ketigaparameter();
}
