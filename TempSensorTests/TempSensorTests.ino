
#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include <OneWire.h>
#include <DallasTemperature.h>

//DS18B20 pin
#define ONE_WIRE_BUS 5

//instance of Dallas sensor
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();


void setup() {
  Serial.begin(9600);
  while(!Serial){} //wait for serial connection
  Serial.println("DS18B20 test");
  sensors.begin();
  Serial.println("DS18B20 begun");

  Serial.println("MCP9808 demo");
  
  // Make sure the sensor is found, you can also pass in a different i2c
  // address with tempsensor.begin(0x19) for example
  if (!tempsensor.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }


}

void loop() {
  Serial.println("wake up MCP9808.... "); // wake up MSP9808 - power consumption ~200 mikro Ampere
  tempsensor.shutdown_wake(0);   // Don't remove this line! required before reading temp
  // Read and print out the temperature, then convert to *F
  float c = tempsensor.readTempC();
  float f = c * 9.0 / 5.0 + 32;
  Serial.print("Temp: "); Serial.print(c,4); Serial.print("C "); 
  Serial.print(f,4); Serial.println("F");
  delay(250);
  
  Serial.println("Shutdown MCP9808.... ");
  tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere
  
  //now the DS1820
  Serial.println("Reading DS1820 ");
  sensors.requestTemperatures(); // Send the command to get temperatures
  c = sensors.getTempCByIndex(0);
  f = c * 9.0 / 5.0 + 32;
  Serial.print("Temp: "); 
  Serial.print(c,4); Serial.print("C "); Serial.print(f,4); Serial.println("F"); 

  delay(1000);

}
