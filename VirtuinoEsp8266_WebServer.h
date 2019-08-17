/* Virtuino ESP01 web server Library ver 1.8.0
 * Created by Ilias Lamprou
 * Updated Aug 17 2019
 * 
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 */



//========= VirtuinoEsp8266 Class methods  
// boolean connectESP8266_toInternet(String wifiNetworkName,String wifiNetworkPassword, int port);  Set your home wifi network SSID and PASSWORD  (Put this function on start of void setup)

// boolean createLocalESP8266_wifiServer(String wifiNetworkName,String wifiNetworkPassword, int port, int mode); Use this function to create an ESP8266 wifi local network
//                                                                    set port to 80 
//                                                                    set mode=2 to use ESP8266 only as access point 
//                                                                    set mode=3 to use ESP8266 as access point and internet station.

//  bool esp8266_setIP(byte a1, byte a2, byte a3, byte a4);           set ESP8266 local IP. Use this function after connectESP8266_toInternet function 

//========= Virtuino general methods  
//  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)       write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
//  int  vDigitalMemoryRead(int digitalMemoryIndex)                   read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
//  void vMemoryWrite(int memoryIndex, float value);                  write a value to Virtuino memory             (memoryIndex=0..31, value range as float value)
//  float vMemoryRead(int memoryIndex);                               read a value of  Virtuino memory             (memoryIndex=0..31, returned a float value
//  run();                                                            neccesary command to communicate with Virtuino android app  (on start of void loop)
//  int getPinValue(int pin);                                         read the value of a Pin. Usefull to read the value of a PWM pin


#ifndef VirtuinoEsp8266_WebServer_h
#define VirtuinoEsp8266_WebServer_h
#include "Arduino.h"



#define ESP8266_USE_SOFTWARE_SERIAL  // disable this command if you want to use hardware serial

//=================   Board configuration ======= 

// #define ESP_GENERIC            // Enable this line to use this library with ESP8266 boards like NodeNCU, WeMos etc.
   

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) 
      #define arduinoAnalogPinsSize 16  
      #define analogInputPinsMap_ { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15 }
      #define wf_arduinoPinsSize 54               // arduino mega board pins count 

#elif defined(ARDUINO_AVR_NANO)
      #define arduinoAnalogPinsSize 8 
      #define analogInputPinsMap_ { A0, A1, A2, A3, A4, A5, A6, A7}
      #define wf_arduinoPinsSize 14               // arduino nano digital pins 
      
#elif defined (__arm__) && defined (__SAM3X8E__)
      #define arduinoAnalogPinsSize 12 
      #define analogInputPinsMap_ { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11}
      #define wf_arduinoPinsSize 54               // arduino due digital pins 

#elif defined(ESP_GENERIC)
      #define arduinoAnalogPinsSize 1 
      #define analogInputPinsMap_ {A0}
      #define wf_arduinoPinsSize 11               //  esp board digital pins 
#else
      #define arduinoAnalogPinsSize 6           // default board
      #define analogInputPinsMap_ { A0, A1, A2, A3, A4, A5}
      #define wf_arduinoPinsSize 14               // arduino uno board pins count 
     
#endif



#ifdef ESP8266_USE_SOFTWARE_SERIAL
#include "SoftwareSerial.h"
#endif


  #define  wf_firmwareCode  "!C00=1.8.0$"                 
  #define  wf_virtualDigitalMemorySize  32     // DV virtual memory size 
  #define  wf_virtualAnalogMemorySize  32      // V virtual memory size 

 #define wf_ERROR_PIN       1
 #define wf_ERROR_VALUE     2
 #define wf_ERROR_TYPE      3
 #define wf_ERROR_SIZE      4
 #define wf_ERROR_PASSWORD  5
 #define wf_ERROR_COMMAND   6
 #define wf_ERROR_UNKNOWN   7

#define wf_COMMAND_START_CHAR '!'
#define wf_COMMAND_END_CHAR   '$'

#define wf_TEXT_COMMAND_MAX_SIZE 80   
  
 class VirtuinoEsp8266_WebServer {
  public:                                            

  #ifdef ESP8266_USE_SOFTWARE_SERIAL
    VirtuinoEsp8266_WebServer(SoftwareSerial &uart);
    VirtuinoEsp8266_WebServer(SoftwareSerial &uart, uint32_t baud);
#else /* HardwareSerial */
    VirtuinoEsp8266_WebServer(HardwareSerial &uart);
    VirtuinoEsp8266_WebServer(HardwareSerial &uart, uint32_t baud);
#endif
    
  void connectESP8266_toInternet(String wifiNetworkName,String wifiNetworkPassword, int port);
  void createLocalESP8266_wifiServer(String wifiNetworkName,String wifiNetworkPassword, int port, int mode=2);
  boolean esp8266_setIP(byte a1, byte a2, byte a3, byte a4);
  void  run();
  
  void vDigitalMemoryWrite(int digitalMemoryIndex, int value);
  int  vDigitalMemoryRead(int digitalMemoryIndex);
  void vMemoryWrite(int memoryIndex, float value);
  float vMemoryRead(int memoryIndex);
  int getPinValue(int pin);
  void vDelay(long milliseconds);
  //void vPinMode(int pin, int mode); 
   long lastCommunicationTime;
  
  boolean DEBUG=false;
  String password=""; 
  const char *NO_REPLY= "_NR_";
   
  String espResponseBuffer="";
    
  boolean waitForResponse(String target1,  int timeout);


  void setTextReceivedCallback(void (*callback)(uint8_t,String)){textReceivedHandler = callback;}
  void setTextRequestedCallback(String (*callback)(uint8_t)){textRequestedHandler = callback;}
 
 
  private:
  
  void checkVirtuinoCommand(String* command);
  boolean wifiSendData(int connectionId, String* content);
  String sendCommand(String command, const int timeout);
  void closeClientConnection(int connectionID);
  void clearESP_buffer(int waitTime);
  char  getCommandType(char c);
  int  getCommandPin(String* aCommand);
  float getCommandValue(String* aCommand);
  String getErrorCommand(byte code);
  void executeReceivedCommand(char activeCommandType, int activeCommandPin ,String* commandString, boolean returnInfo);
  
  
  // byte arduinoPinsMap[arduinoPinsSize];
  byte arduinoPinsValue[wf_arduinoPinsSize];
  int virtualDigitalMemory[wf_virtualDigitalMemorySize];                            
  float virtualFloatMemory[wf_virtualAnalogMemorySize];                               
  byte analogInputPinsMap[arduinoAnalogPinsSize]  = analogInputPinsMap_  ;
 
   String espBuffer="";
   
   
   String urlencode(String* str);
   unsigned char h2int(char c);
   String urldecode(String* str);
  
  void (*textReceivedHandler)(uint8_t,String);
  String (*textRequestedHandler)(uint8_t); 
   
#ifdef ESP8266_USE_SOFTWARE_SERIAL
    SoftwareSerial *espSerial; /* The UART to communicate with ESP01 */
#else
    HardwareSerial *espSerial; /* The UART to communicate with ESP01 */
#endif                                                                 

 };


#endif
