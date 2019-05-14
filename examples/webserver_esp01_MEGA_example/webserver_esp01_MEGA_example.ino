
/* Virtuino ESP8266 web server Arduino MEGA - DUE example
 * Example name = "Basic code"
 * Created by Ilias Lamprou
 * Updated 23 02 2017
 * Before  running this code config the settings below as the instructions on the right
 * 
 * 
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino&hl=el
 * Getting starting link:
 * Video tutorial link: 
 * Contact address for questions or comments: iliaslampr@gmail.com
 */

/*========= VirtuinoEsp8266 Class methods  
 
 * boolean connectESP8266_toInternet(String wifiNetworkName,String wifiNetworkPassword, int port);  Set your home wifi network SSID and PASSWORD  (Put this function on start of void setup)
 *
 * boolean createLocalESP8266_wifiServer(String wifiNetworkName,String wifiNetworkPassword, int port, int mode); Use this function to create an ESP8266 wifi local network
 *                                                                   set port to 80 
 *                                                                   set mode=2 to use ESP8266 only as access point 
 *                                                                   set mode=3 to use ESP8266 as access point and internet station.
 *
 *  bool esp8266_setIP(byte a1, byte a2, byte a3, byte a4);           set ESP8266 local IP. Use this function after connectESP8266_toInternet function 
 *
 * ========= Virtuino general methods  
 *  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)       write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
 *  int  vDigitalMemoryRead(int digitalMemoryIndex)                   read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
 *  void vMemoryWrite(int memoryIndex, float value);                  write a value to Virtuino memory             (memoryIndex=0..31, value range as float value)
 *  float vMemoryRead(int memoryIndex);                               read a value of  Virtuino memory             (memoryIndex=0..31, returned a float value
 *  run();                                                            neccesary command to communicate with Virtuino android app  (on start of void loop)
 *  int getPinValue(int pin);                                         read the value of a Pin. Usefull to read the value of a PWM pin
 *  void vDelay(long milliseconds);                                   Pauses the program (without block communication) for the amount of time (in miliseconds) specified as parameter
 *  void vDelay(long milliseconds);                                   Pauses the program (without block communication) for the amount of time (in miliseconds) specified as parameter
 *  void clearTextBuffer();                                           Clear the text received text buffer
 *  int textAvailable();                                              Check if there is text in the received buffer
 *  String getText(byte ID);                                          Read the text from Virtuino app
 * void sendText(byte ID, String text);                               Send text to Virtuino app  
 */
 
#include "VirtuinoEsp8266_WebServer.h"

 VirtuinoEsp8266_WebServer virtuino(Serial1);    

// Connect the  ESP01 module to Arduino Mega - Due Serial1 port as the plan below
//  http://iliaslamprou.mysch.gr/virtuino/esp8266_arduino_mega_connection_plan.png

// Arduino Mega settings -> Open VirtuinoEsp8266_WebServer file on the virtuino library folder ->  disable the line: #define ESP8266_USE_SOFTWARE_SERIAL 

// Arduino Due settings  -> 1.Open VirtuinoBluetooth.h on the virtuino library folder ->  disable the line: #define BLUETOOTH_USE_SOFTWARE_SERIAL 
//                       -> 2.Open VirtuinoEsp8266_WebServer file on the virtuino library folder ->  disable the line: #define ESP8266_USE_SOFTWARE_SERIAL 
                               




//================================================================== setup
//==================================================================
//==================================================================
void setup() 
{
  virtuino.DEBUG=true;                                            // set this value TRUE to enable the serial monitor status.It is neccesary to get your esp8266 local ip
  Serial.begin(9600);                                             // Enable this line only if DEBUG=true

  Serial1.begin(115200);
  
  virtuino.connectESP8266_toInternet("WIFI NETWORK","WIFI_PASSWORD",8000);  // Set your home wifi router SSID and PASSWORD. ESP8266 will connect to Internet. Port=80
  virtuino.esp8266_setIP(192,168,1,150);                                          // Set a local ip. Forward port 80 to this IP on your router

  //virtuino.createLocalESP8266_wifiServer("ESP8266 NETWORK NAME","PASSWORD",80,2);   //Enable this line to create a wifi local netrork using ESP8266 as access point
                                                                                      //Do not use less than eight characters for the password. Port=80
                                                                                      //Default access point ESP8266 ip=192.168.4.1. 
 
  virtuino.password="1234";                                     // Set a password to your web server for more protection 
                                                                // avoid special characters like ! $ = @ # % & * on your password. Use only numbers or text characters
                              
  
  
//------ enter your setup code below

   pinMode(13,OUTPUT);       
 
    
    
}


//================================================================== loop
//==================================================================
//==================================================================


void loop(){
   virtuino.run();           //  necessary command to communicate with Virtuino android app
   
    //------ enter your loop code below here
    //------ avoid to use delay() function in your code. Use the command virtuino.vDelay() instead of delay()

    // your code .....

   
   
        
     //----- end of your code
 }


