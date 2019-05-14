//-- ThingSpeak ethernet shield arduino example by Ilias Lamprou
//-- Use Virtuino app to read or write to thingSpeak server
//-- Video tutorial: https://www.youtube.com/watch?v=ng767bg2BNk
//-- Download latest Virtuino android app from the link:https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino&hl
//-- 08/22/2016

#include <DHT.h>        // You have to download DHT11  library
                        //Attention: For new  DHT11 version  library you will need the Adafruit_Sensor library
                        //Download from here: https://github.com/adafruit/Adafruit_Sensor




//Attention: For new  DHT11 version  libraries you will need the Adafruit_Sensor library
//Download from here:https://github.com/adafruit/Adafruit_Sensor
//and install to Arduino software


#include <SPI.h> 
#include <Ethernet.h>
 
  #ifdef SPARK
    #include "ThingSpeak/ThingSpeak.h"    // You have to download ThingSpeak  library
  #else
    #include "ThingSpeak.h"
  #endif

#define WRITE_DELAY_FOR_THINGSPEAK 15000
#define DHTPIN 5   
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
  

  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE,0xED};
  IPAddress ip(192,168,2,150);                             //Check your router IP
  EthernetClient client;

  
  unsigned long mySensorsChannelNumber = 146094;            //Replase this code with your thingSpeak channel ID
  unsigned long myCommandsChannelNumber = 146096;           //Replase this code with your thingSpeak channel ID
  
  const char * myWriteAPIKey_sensors = "LCOP8RJNAWSIJ8NG";  //Replace with your channel Write Key
  const char * myWriteAPIKey_commands = "NA2E6X99WU4PG9G1"; //Replace with your channel Write Key. This key is not used in this example
                                                            //Use this key from android device  

long lastWriteTime=0;    
long lastReadTime=0;
float responseValue=0;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  ThingSpeak.begin(client);
  dht.begin();
  pinMode(7,OUTPUT);
  Serial.println("Setup completed");
  delay(1000);
}


void loop() {
  
  //-------- Example 1 write DHT11 sensors' values to thingSpeak channel sensors
  
  if (millis()-lastWriteTime>20000) {       // every 20 secs.  Must be >15000.    
      Serial.println("Read sensors' values...");
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      if (isnan(t) || isnan(h)) {
        Serial.println("Failed to read from DHT");
      }
      else {
          Serial.println("Temp="+String(t)+" *C");
          Serial.println("Humidity="+String(h)+" %");
          ThingSpeak.setField(1,t);
          ThingSpeak.setField(2,h);
          ThingSpeak.setField(3,responseValue);             // This is the value of commands channel
          ThingSpeak.writeFields(mySensorsChannelNumber, myWriteAPIKey_sensors);                  // write two values
          lastWriteTime=millis();               // store last write time
      }
  }

  //-------- Example 2 read value from second channel "Commands/field1"
  //-------- if value is 1 enable pin 13 board. Write the value to the other channel (Sensors/field 3) as responce
 
   if (millis()-lastReadTime>15000) {       // read value every 15 secs. Avoid delay
      float value = ThingSpeak.readFloatField(myCommandsChannelNumber, 1);
      Serial.println("Read thingSpeak channel commands/field 1 last value="+ String(value));
      if (value==0) digitalWrite(7,LOW);
      else digitalWrite(7,HIGH);
      responseValue=value;
      lastReadTime=millis();                // store last read time
   }
   
 
 
} 



