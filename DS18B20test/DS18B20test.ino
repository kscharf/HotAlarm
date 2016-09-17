
#include <OneWire.h>
#include <DallasTemperature.h>
//DS18B20 pin
#define ONE_WIRE_BUS 5

//instance of Dallas sensor
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


int isNotEqual(float a, float b)
{
  if(round(a*1000.0) != round(b*1000.0))
    return 1;
  else
    return 0;
}

float previous;
void setup() {
  previous=0;

  Serial.begin(9600);
  while(!Serial){} //wait for serial connection
  Serial.println("DS18B20 test");

  sensors.begin();

  Serial.println("DS18B20 begun");

}

void loop() {
// call sensors.requestTemperatures() to issue a global temperature
// request to all devices on the bus
float c,f;


//  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
//  Serial.println("DONE");
  c = sensors.getTempCByIndex(0);
  f = c * 9.0 / 5.0 + 32;

//  if(isNotEqual(f,previous)){  //new value
    previous = f; //set new value as old
    Serial.print("Temperature for Device 1 is: ");
    Serial.print(c,8); Serial.print("C "); Serial.print(f,8); Serial.println("F"); 
//  }
}
