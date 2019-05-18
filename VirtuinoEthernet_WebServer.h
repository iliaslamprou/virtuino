/* Virtuino Ethernet Shield web server library ver 1.7.1
 * Created by Ilias Lamprou
 * Modify by Rico Schumann
 * Updated 16/05/2019
 * 
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.javapapers.android.agrofarmlitetrial ???
 * Getting starting link:
 * Video tutorial link: 
 * Contact address for questions or comments: iliaslampr@gmail.com
 */



//========= Virtuino General methods  

//  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)       write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
//  int  vDigitalMemoryRead(int digitalMemoryIndex)                   read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
//  void vMemoryWrite(int memoryIndex, float value);                  write a value to Virtuino memory             (memoryIndex=0..31, value range as float value)
//  float vMemoryRead(int memoryIndex);                               read a value of  Virtuino memory             (memoryIndex=0..31, returned a float value
//  run();                                                            neccesary command to communicate with Virtuino android app  (on start of void loop)
//  int getPinValue(int pin);                                         read the value of a Pin. Usefull to read the value of a PWM pin
//  void clearTextBuffer();                                       Clear the text received text buffer
//  int textAvailable();                                          Check if there is text in the received buffer
//  String getText(byte ID);                                      Read the text from Virtuino app
//  void sendText(byte ID, String text);                          Send text to Virtuino app  




#ifndef VirtuinoEthernet_WebServer_h
#define VirtuinoEthernet_WebServer_h


#include "Arduino.h"
#include <SPI.h>
#include <Ethernet.h>    
//#include <Ethernet2.h>


//=================   Board configuration ======= 

//#define ESP_GENERIC            // Enable this line to use this library with ESP8266 boards like NodeNCU, WeMos etc.
 

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) 
      #define arduinoAnalogPinsSize 16  
      #define analogInputPinsMap_ { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15 }
      #define et_arduinoPinsSize 54               // arduino mega board pins count 

#elif defined(ARDUINO_AVR_NANO)
      #define arduinoAnalogPinsSize 8 
      #define analogInputPinsMap_ { A0, A1, A2, A3, A4, A5, A6, A7}
      #define et_arduinoPinsSize 14               // arduino nano digital pins 
      
#elif defined (__arm__) && defined (__SAM3X8E__)
      #define arduinoAnalogPinsSize 12 
      #define analogInputPinsMap_ { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11}
      #define et_arduinoPinsSize 54               // arduino due digital pins 

#elif defined(ESP_GENERIC)
      #define arduinoAnalogPinsSize 1 
      #define analogInputPinsMap_ {A0}
      #define et_arduinoPinsSize 11               //  esp board digital pins 
#else
      #define arduinoAnalogPinsSize 6           // default board
      #define analogInputPinsMap_ { A0, A1, A2, A3, A4, A5}
      #define et_arduinoPinsSize 14               // arduino uno board pins count 
     
#endif



  #define  et_firmwareCode  "!C00=1.7.1$"                 
  #define  et_virtualDigitalMemorySize  32       // DV virtual memory size 
  #define  et_virtualAnalogMemorySize  32        // V virtual memory size 


#define et_ERROR_PIN       1
#define et_ERROR_VALUE     2
#define et_ERROR_TYPE      3
#define et_ERROR_SIZE      4
#define et_ERROR_PASSWORD  5
#define et_ERROR_COMMAND   6
#define et_ERROR_UNKNOWN   7

#define et_COMMAND_START_CHAR '!'
#define et_COMMAND_END_CHAR   '$'
#define et_COMMAND_ERROR       "E00="


#define et_TEXT_COMMAND_MAX_SIZE 80   

#define et_clientConnected  "Connected"
#define et_clientDisconnected  "Disconnected"

 class VirtuinoEthernet_WebServer {
  public:
    VirtuinoEthernet_WebServer(EthernetServer *VirtuinoServer);
    void run();
    void vDigitalMemoryWrite(int digitalMemoryIndex, int value);
    int  vDigitalMemoryRead(int digitalMemoryIndex);
    void vMemoryWrite(int memoryIndex, float value);
    float vMemoryRead(int memoryIndex);
    int getPinValue(int pin);
    void vDelay(long milliseconds);
    //void vPinMode(int pin, int mode); 
    boolean DEBUG=false;
    String password=""; 
    unsigned long lastCommunicationTime;
    
    //-- Text Command public functions
    void clearTextBuffer();
    int textAvailable();
    String getText(byte ID);
    void sendText(byte ID, String text);
    String etResponseBuffer="";
  
  
    private:
	  EthernetServer *MyServer; 
    void checkVirtuinoCommand(String* command);
    char  getCommandType(char c);
    int  getCommandPin(String* aCommand);
    float getCommandValue(String* aCommand);
    String getErrorCommand(byte code);
    void executeReceivedCommand(char activeCommandType, int activeCommandPin ,String* commandString, boolean returnInfo);

    byte arduinoPinsValue[et_arduinoPinsSize];
    int virtualDigitalMemory[et_virtualDigitalMemorySize];                            
    float virtualFloatMemory[et_virtualAnalogMemorySize];                               
    byte analogInputPinsMap[arduinoAnalogPinsSize]  = analogInputPinsMap_  ;
    int serverPort=80;
    String lineBuffer="";
    

    //-- Text Command private functions
    String textReceivedCommandBuffer="";
    String textToSendCommandBuffer="";
    String urlencode(String* str);
    unsigned char h2int(char c);
    String urldecode(String* str);
    void clearTextByID(byte ID, String* textBuffer);
    void addTextToReceivedBuffer(byte ID, String* text);

  
 };



 
#endif
