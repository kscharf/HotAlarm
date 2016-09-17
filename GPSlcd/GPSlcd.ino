/**
 *  ___ ___  _  _   _     ___  __  ___    ___ ___  ___
 * | __/ _ \| \| | /_\   ( _ )/  \( _ )  / __| _ \/ __|
 * | _| (_) | .` |/ _ \  / _ \ () / _ \ | (_ |  _/\__ \
 * |_| \___/|_|\_/_/ \_\ \___/\__/\___/  \___|_|  |___/
 *
 * This example is meant to work with the Adafruit
 * FONA 808 or 3G Shield or Breakout
 *
 * Copyright: 2015 Adafruit
 * Author: Todd Treece
 * Licence: MIT
 *
 */
//include fona libarary
#include "Adafruit_FONA.h"

// include the LCD library code:
#include <Wire.h>
#include <LiquidTWI.h>
// Connect via i2c, default address #0 (A0-A2 not jumpered)
LiquidTWI lcd(0);

// standard pins for the shield, adjust as necessary
//#define FONA_RX 2
//#define FONA_TX 3
#define FONA_RST 6

// We default to using software serial. If you want to use hardware serial
// (because softserial isnt supported) comment out the following three lines 
// and uncomment the HardwareSerial line
//#include <SoftwareSerial.h>
//SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
//SoftwareSerial *fonaSerial = &fonaSS;

// Hardware serial is also possible!
HardwareSerial *fonaSerial = &Serial1;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

// Have a FONA 3G? use this object type instead
//Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);


void setup() {

#if 0
  while (! Serial);

  Serial.begin(115200);
  Serial.println(F("Adafruit FONA 808 & 3G GPS demo"));
  Serial.println(F("Initializing FONA... (May take a few seconds)"));
#endif

  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Fona GPS Test");


  fonaSerial->begin(4800);

  lcd.setCursor(0,1); //second line
  if (! fona.begin(*fonaSerial)) {
    lcd.print("Couldn't find FONA");
    while(1);
  }
  lcd.print("FONA is OK");

  
  
  // Try to enable GPRS
  lcd.setCursor(0,0);
  lcd.print("Enabling GPS...");
  fona.enableGPS(true);
}

void loop() {
  static int wait4fix = 0;
  delay(2000);

  float latitude, longitude, speed_kph, heading, speed_mph, altitude;

  // if you ask for an altitude reading, getGPS will return false if there isn't a 3D fix
//  boolean gps_success = fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude);
  boolean gps_success = fona.getGPS(&latitude, &longitude);

  lcd.setCursor(0,0);

  if (gps_success) {
    wait4fix=0;
    lcd.clear();
    lcd.print("GPS lat:");
    lcd.print(latitude, 6);
    lcd.setCursor(0,1);
    lcd.print("GPS long:");
    lcd.print(longitude, 6);
#if 0
    Serial.print("GPS speed KPH:");
    Serial.println(speed_kph);
    Serial.print("GPS speed MPH:");
    speed_mph = speed_kph * 0.621371192;
    Serial.println(speed_mph);
    Serial.print("GPS heading:");
    Serial.println(heading);
    Serial.print("GPS altitude:");
    Serial.println(altitude);
#endif
  } else {
    lcd.clear();
    lcd.print("no GPS fix      ");
    lcd.setCursor(0,1);
    lcd.print(wait4fix++);
  }


  delay(2000);
  // Fona 3G doesnt have GPRSlocation :/
  if ((fona.type() == FONA3G_A) || (fona.type() == FONA3G_E))
    return;
  // Check for network, then GPRS
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Cell network???");
  if (fona.getNetworkStatus() == 1) {
    // network & GPRS? Great! Print out the GSM location to compare
    boolean gsmloc_success = fona.getGSMLoc(&latitude, &longitude);

    if (gsmloc_success) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("GSM lat:");
      lcd.print(latitude, 6);
      lcd.setCursor(0,1);
      lcd.print("GSM long:");
      lcd.println(longitude, 6);
    } else {
      lcd.setCursor(0,1);
      lcd.print("GSM loc failed");
//      Serial.println(F("Disabling GPRS"));
      fona.enableGPRS(false);
//      Serial.println(F("Enabling GPRS"));
      if (!fona.enableGPRS(true)) {
        lcd.setCursor(0,1);
        lcd.print("Fail GPRS on");  
      }
    }
  }
  else{
    lcd.setCursor(0,1);
    lcd.print("NO Cell network!");
  }
  delay(1000);
  
}

