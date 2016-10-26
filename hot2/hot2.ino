#define NOSERIAL true
//#define NONETWORK
//#define NOSMS

// NOSMS implied by NONETWORK, but can be defined by itself too
#ifdef NONETWORk
  #define NOSMS true
#endif  

//90 degrees F
#define ALARMTEMP 900
#define TEMPDEBOUNCE 5
#define TEXTTO "9542581009"
#define ALARMMESSAGE "_TempAlarm_"
#define RETRIGGER  TEMPDEBOUNCE


/* libraries-------------------------------------------------- */
//include fona libarary
#include "Adafruit_FONA.h"

// include the LCD library code:
#include <Wire.h>
#include <LiquidTWI.h>

//include the onewire and Dallas sensor libraries
#include <OneWire.h>
#include <DallasTemperature.h>

/*------------temperature sensor stuff---------------------*/
//DS18B20 pin
#define ONE_WIRE_BUS 5

// standard pins for the shield, adjust as necessary
//#define FONA_RX 2
//#define FONA_TX 3
#define FONA_RST 6
#define FONA_KEY 9
#define FONA_PWR_STAT 10


//instance of Dallas sensor
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// --------------LCD------------------------------
// Connect via i2c, default address #0 (A0-A2 not jumpered)
LiquidTWI lcd(0);


//---------fona------------------------
// this is a large buffer for replies
char replybuffer[255];
// Hardware serial is also possible!
HardwareSerial *fonaSerial = &Serial1;
// Use this for FONA 800 and 808s
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
uint8_t type;
uint8_t retrigger_count=0;

bool bAlarm = false;
bool bOverTemp = false;
bool bAlarmSent = false;

uint8_t TempDebounce = 0;

/*-------------------sketch setup routine---------------*/
void setup() {
  pinMode(FONA_KEY,OUTPUT);
  digitalWrite(FONA_KEY,0);
  pinMode(FONA_PWR_STAT,INPUT);
  
  
  /*init the serial port for monitor, comment out or test for USB plugged in to operate without*/
#ifndef NOSERIAL
  Serial.begin(9600);
  while(!Serial){} //wait for serial connection
#endif
  //dallas sensor
  sensors.begin(); //init the dallas sensor

  //LCD
  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);

  lcd.setCursor(0,0);
  lcd.print("FONA ON");


  digitalWrite(FONA_KEY,1); //turn on fona via transistor
  while(digitalRead(FONA_PWR_STAT) == 0){}
  digitalWrite(FONA_KEY,0);



  //FONA
  fonaSerial->begin(4800);
  lcd.setCursor(0,0);
  lcd.print("Looking for FONA");
  if (! fona.begin(*fonaSerial)) {
    lcd.setCursor(0,0);
    lcd.print("-NO FONA FOUND-");
#ifndef NOSERIAL
    Serial.println(F("Couldn't find FONA"));
#endif
    while (1);
  }
  type = fona.type();
}


void loop() {
uint8_t netstatus;
char sAlarmMsg[80];
uint16_t vbat;

  lcd.clear();

  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  float c,f;
  int temp;

  sensors.requestTemperatures(); // Send the command to get temperatures
  c = sensors.getTempCByIndex(0);
  f = c * 9.0 / 5.0 + 32;
  temp = f*10;

#ifndef NOSERIAL
    Serial.print(f,8);Serial.print("F ");Serial.println(temp); 
#endif  
  lcd.setCursor(0,1);
  lcd.print(f);lcd.print("F ");lcd.print(temp);
  
  
  if(bOverTemp == false) //no trigger..yet
  {
    if(temp > ALARMTEMP)
    { //alarm trigger
      bOverTemp = true;
#ifndef NOSERIAL
      Serial.println("overtemp set");
#endif
    }
    
  }
  
  if(bOverTemp == true)
  {
    if(temp > ALARMTEMP) 
    {//still true
      if(TempDebounce++ > TEMPDEBOUNCE) 
      {
	      bAlarm = true; //set alarm
        lcd.setCursor(8,0); lcd.print("ALRM");
#ifndef NOSERIAL
        Serial.println("Alarm Set");
#endif                
      }
    }else{
      TempDebounce=0;
      bOverTemp = false;
    }
  }  
  
  if((bAlarmSent==true)&&(bOverTemp==false)){
    if(++retrigger_count > RETRIGGER)
    {
      bAlarm = false;	//reset to retrigger
      bAlarmSent = false;
      retrigger_count=0;
    }
  }

  
#ifndef NONETWORK
  //test network status
  netstatus = fona.getNetworkStatus();
  uint8_t n = netstatus;
  lcd.setCursor(0,0);
  lcd.print("Net   ");
  lcd.setCursor(3,0); lcd.print(n); //want to see 1, others are 'bad'
#ifndef NOSERIAL
  Serial.print(F("Network status "));
  Serial.print(n);
  Serial.print(F(": "));
  if (n == 0) Serial.println(F("Not registered"));
  if (n == 1) Serial.println(F("Registered (home)"));
  if (n == 2) Serial.println(F("Not registered (searching)"));
  if (n == 3) Serial.println(F("Denied"));
  if (n == 4) Serial.println(F("Unknown"));
  if (n == 5) Serial.println(F("Registered roaming"));
#endif

  // read the RSSI
  n = fona.getRSSI();
  int8_t r;

  lcd.setCursor(4,0);lcd.print("-  ");lcd.setCursor(5,0);
  lcd.print(n);	//so net is Net X-SS for status-rel rssi (0-30, higher is better, 5 is "good enough")
 
#ifndef NOSERIAL
  Serial.print(F("RSSI = ")); Serial.print(n); Serial.print(": ");
  if (n == 0) r = -115;
  if (n == 1) r = -111;
  if (n == 31) r = -52;
  if ((n >= 2) && (n <= 30)) {
    r = map(n, 2, 30, -110, -54);
  }
  Serial.print(r); Serial.println(F(" dBm"));
#endif  
#endif

  //Alarm condition now met?
  if((bAlarm==true)&&(bAlarmSent==false))
  { //alarm triggered, build alarm string
    sprintf(sAlarmMsg,"%s%dF",ALARMMESSAGE,temp);

#ifdef NOSMS
#ifndef NOSERIAL
    Serial.println(sAlarmMsg);
#endif
    lcd.setCursor(8,0); lcd.print("ALRMsent");
    bAlarmSent = true;
#else
    if((netstatus==1)||(netstatus==5)){  //registered home or roaming
      if(fona.sendSMS(TEXTTO, sAlarmMsg)){
        bAlarmSent = true;
        lcd.setCursor(8,0); lcd.print("ALRMsent");
      }else{
        lcd.setCursor(8,0); lcd.print("ALRMrtry");
      }
    }else{
      lcd.setCursor(8,0); lcd.print("ALRMpend");
    }  
#endif      
  }

//---keep alarm status until retriggered
  if((bAlarmSent==true)&&(bAlarm==true)){
    lcd.setCursor(8,0); lcd.print("alrm");
  }
  lcd.setCursor(14,0);  
  if(bOverTemp==true)
    lcd.print("!");
  else
    lcd.print(" ");   
  if(bAlarmSent==true)
    lcd.print("$"); 


//battery percent left
  vbat = 0;
  fona.getBattPercent(&vbat);
  if(vbat){
    lcd.setCursor(12,1);
    lcd.print(vbat);
    lcd.print("%");
  }
             
  
  delay(2000); //two second loop
}
