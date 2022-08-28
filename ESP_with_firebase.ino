#include <FirebaseESP32.h>
#include  <WiFi.h>
#include "DHT.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

#define FIREBASE_HOST "https://esp32-database-670f0-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define WIFI_SSID "Router129" // Change the name of your WIFI
#define WIFI_PASSWORD "1Jz18jA23MN" // Change the password of your WIFI
#define FIREBASE_Authorization_key "AIzaSyCExj6dTHBpz2PVZfsXcBDLS2ThqfVhS_Q"

#define DHTPIN 15    

#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

const long utcOffsetInSeconds = 25200;
FirebaseData firebaseData;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);
char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};


void setup() {

 Serial.begin(9600);
  dht.begin();
   WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
   Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST,FIREBASE_Authorization_key);
  timeClient.begin();
}

void loop() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  char str[20];
  float hum = dht.readHumidity();
  float temp = dht.readTemperature(); 

  if (isnan(hum) || isnan(temp)  ){
    Serial.println(F("Pembacaan Sensor Gagal"));
    return;
  }

  int jam = timeClient.getHours();
  int menit = timeClient.getMinutes();
  int detik = timeClient.getSeconds();
  String waktu = String(jam)+":"+String(menit)+":"+String(detik);
  
  String hari = daysOfTheWeek[timeClient.getDay()];
  int currentYear = ptm->tm_year+1900;
  int currentMonth = ptm->tm_mon+1;
  int monthDay = ptm->tm_mday;
  String tanggal = String(monthDay)+ "-" + String(currentMonth) + "-" + String(currentYear);

  int idDevice = ESP.getEfuseMac();
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("°C");
  Serial.print(" Humidity: ");
  Serial.print(hum);
  Serial.print("%");
  Serial.println();

  Firebase.setString(firebaseData, "/ESP32_APP/1. HARI", hari);
  Firebase.setString(firebaseData, "/ESP32_APP/3. JAM", waktu);
  Firebase.setString(firebaseData, "/ESP32_APP/2. TANGGAL", tanggal);
  Firebase.setFloat(firebaseData, "/ESP32_APP/4. SUHU", temp);
  Firebase.setInt(firebaseData, "/ESP32_APP/3. DEVICE", idDevice);
  
   delay(1000);
}
