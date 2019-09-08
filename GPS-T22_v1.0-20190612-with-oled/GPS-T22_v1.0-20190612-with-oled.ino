/*****************************************
  ESP32 GPS VKEL 9600 Bds
This version is for T22_v01 20190612 board
As the power management chipset changed, it
require the axp20x library that can be found
https://github.com/lewisxhe/AXP202X_Library
You must import it as gzip in sketch submenu
in Arduino IDE
This way, it is required to power up the GPS
module, before trying to read it.

Also get TinyGPS++ library from: 
https://github.com/mikalhart/TinyGPSPlus
******************************************/

#include <TinyGPS++.h>
#include <axp20x.h>
#include <Wire.h>
#include "SSD1306.h"
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin

SSD1306 display(0x3c, 21, 22);
TinyGPSPlus gps;
HardwareSerial GPS(1);
AXP20X_Class axp;


void setup()
{
  pinMode(16,OUTPUT);
  pinMode(2,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  
  
  Serial.begin(115200);
  Wire.begin(21, 22);
  if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) {
    Serial.println("AXP192 Begin PASS");
  } else {
    Serial.println("AXP192 Begin FAIL");
  }
  axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
  axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
  axp.setPowerOutPut(AXP192_DCDC2, AXP202_ON);
  axp.setPowerOutPut(AXP192_EXTEN, AXP202_ON);
  axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);
  GPS.begin(9600, SERIAL_8N1, 34, 12);   //17-TX 18-RX

  IntroDisplay();

}

void loop()
{
  static const double LONDON_LAT = 48.790272, LONDON_LON = 2.050810;
  float distanceKmToLondon =
    (float)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      LONDON_LAT, 
      LONDON_LON) / 1000;
  Serial.print("Dist-2-sqy: ");
  Serial.print(distanceKmToLondon, 3);
  Serial.println("Km");
  SerialPrintGps();
  gpsDiplay(distanceKmToLondon);
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (GPS.available())
      gps.encode(GPS.read());
  } while (millis() - start < ms);
}

static void SerialPrintGps()
{
  Serial.print("Latitude  : ");
  Serial.println(gps.location.lat(), 5);
  Serial.print("Longitude : ");
  Serial.println(gps.location.lng(), 4);
  Serial.print("Satellites: ");
  Serial.println(gps.satellites.value());
  Serial.print("Altitude  : ");
  Serial.print(gps.altitude.meters());
  Serial.println("M");
  Serial.print("Time      : ");
  Serial.print(gps.time.hour());
  Serial.print(":");
  Serial.print(gps.time.minute());
  Serial.print(":");
  Serial.println(gps.time.second());
  Serial.print("Speed     : ");
  Serial.println(gps.speed.kmph(), 2); 
  Serial.println("**********************");
}

static void gpsDiplay(float plop)
{
  char buff[10];
  char lati[10];
  char longi[10];
  char sat[3];
  char timeG[9];
  char *cur=timeG, * const endbuff = timeG + sizeof timeG;
  char speedGPS[10];
  char altitudeGPS[10]; 
  snprintf (altitudeGPS, sizeof(altitudeGPS), "%f", gps.altitude.meters()); 
  snprintf (buff, sizeof(buff), "%f", plop);
  snprintf(lati, sizeof(lati), "%f", gps.location.lat());
  snprintf(longi, sizeof(longi), "%f", gps.location.lng()); 
  snprintf(sat, sizeof(sat), "%i", gps.satellites.value());
  cur += snprintf(cur ,endbuff-cur, "%02i:", gps.time.hour());
  cur += snprintf(cur, endbuff-cur, "%02i:",  gps.time.minute());
  cur += snprintf(cur, endbuff-cur, "%02i",gps.time.second());
  snprintf(speedGPS, sizeof(speedGPS), "%.2f", gps.speed.kmph()); 
  display.clear();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0,0, "Dist-2-work : ");
  display.drawString(60,0, buff);
  display.drawString(0,9, "Latitude : ");
  display.drawString(60,9, lati);
  display.drawString(0,18, "Longitude : ");
  display.drawString(60,18, longi);
  display.drawString(0,27, "Altitude : ");
  display.drawString(60,27, altitudeGPS);  
  display.drawString(0,36, "#Satellites : ");
  display.drawString(60,36, sat);
  display.drawString(0,45, "Time : ");
  display.drawString(60,45, timeG);
  display.drawString(0,54, "Speed : ");
  display.drawString(60,54, speedGPS);
  display.display();
}

static void IntroDisplay()
{
  display.init();
  display.flipScreenVertically();  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
// display.setFont(ssd1306xled_font6x8);
  display.drawString(64 , 0 , "Proto Netmonk");
  display.drawString(64 , 20 , "GPS TRACKER");
  display.drawString(64 , 40 , "Ready!");
  display.display();
}
