
/* Virtuino Ethernet Shield web server example No1  
 * Example name = "Enable or disable pin 13 LED"
 * Created by Ilias Lamprou
 * Updated May 18 2018
 * Before  running this code config the settings below as the instructions on the right
 * 
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino&hl=el
 */


/*========= Virtuino General methods  
*
*  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)       write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
*  int  vDigitalMemoryRead(int digitalMemoryIndex)                   read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
*  void vMemoryWrite(int memoryIndex, float value);                  write a value to Virtuino memory             (memoryIndex=0..31, value range as float value)
*  float vMemoryRead(int memoryIndex);                               read a value of  Virtuino memory             (memoryIndex=0..31, returned a float value
*  run();                                                            neccesary command to communicate with Virtuino android app  (on start of void loop)
*  int getPinValue(int pin);                                         read the value of a Pin. Usefull to read the value of a PWM pin
*  void vDelay(long milliseconds);                                   Pauses the program (without block communication) for the amount of time (in miliseconds) specified as parameter
*  void clearTextBuffer();                                           Clear the text received text buffer
*  int textAvailable();                                              Check if there is text in the received buffer
*  String getText(byte ID);                                          Read the text from Virtuino app
*  void sendText(byte ID, String text);                              Send text to Virtuino app  

*/ 

#include "VirtuinoEthernet_WebServer.h"                           // Neccesary virtuino library for ethernet shield

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};                // Set the ethernet shield mac address.
IPAddress ip(192, 168, 1, 150);                                   // Set the ethernet shield ip address. Check your gateway ip address first
 
EthernetServer VirtuinoServer(8000);                              // default port=8000
VirtuinoEthernet_WebServer virtuino(&VirtuinoServer);  


//--- Declare some text variables. These are the variables for Virtuino T pins
String T0 ="";    
String T1 ="";
//String T2 ="";
//String T3 ="";


//================================================================== setup
//==================================================================
//==================================================================
void setup(){
  
  virtuino.DEBUG=true;                                            // set this value TRUE to enable the serial monitor status
  Serial.begin(9600);                                             // Enable this line only if DEBUG=true
                                     
  Ethernet.begin(mac, ip);
  VirtuinoServer.begin();
  virtuino.password="1234";                                     // Set a password to your web server for more protection
                                                                // avoid special characters like ! $ = @ # % & * on your password. Use only numbers or text characters
  virtuino.setTextReceivedCallback(onTextReceived);
  virtuino.setTextRequestedCallback(onTextRequested);

  Serial.println("Server Ready");
  Serial.println(Ethernet.localIP());                            
 
  
//------ enter your setup code below

   pinMode(6,OUTPUT);                                           // Connect a Led to pin 6
                                                                // Don't use pins 10,11,12,13 in your code. They used by Ethernet Shield

   
                                                  
}


//================================================================== loop
//==================================================================
//==================================================================


void loop(){
   virtuino.run();           //  necessary command to communicate with Virtuino app
   
    //------ enter your loop code below here
    //------ avoid to use delay() function in your code. Use the command virtuino.vDelay() instead of delay()

    // your code .....

      T0="Hello Virtuino "+String(random(100));    // on Virtuino panel add a "Text Value Display" to get this text
   
      virtuino.vDelay(5000);    // use this instead of the default delay(5000)
 




        
     //----- end of your code
 }

 //=======================================================
/* This void is called by Virtuino library any time it receives a text from Virtuino app
   You have to store the text to a variable and then to do something with the text*/
void onTextReceived(uint8_t textPin, String receivedText){
   Serial.println("====Received T"+String(textPin)+"="+receivedText);
   if (textPin==0) T0=receivedText;
   else if (textPin==1) T1=receivedText;
   //else if (textPin==2) T2=receivedText;

   //else if (textPin==3) {
   //     T3=receivedText;
   //     if (T3=="TURN ON") digitalWrite(.....
   // }
}

//=======================================================
/* Every time Virtuino app requests about a text of a T pin, the library calls this function
   You have to return the string for which the app requests the text*/
String onTextRequested(uint8_t textPin){
   Serial.println("==== Requested T"+String(textPin));
   if (textPin==0) return T0;
   else if (textPin==1) return T1;
   //else if (textPin==2) return T2;
   
   return virtuino.NO_REPLY;
}
