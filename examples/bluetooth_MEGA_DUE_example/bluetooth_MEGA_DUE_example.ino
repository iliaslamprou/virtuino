/* Virtuino example Arduino Mega, DUE or others with Hardware Serial (Bluetooth)
 * Example name = "Basic Virtuino code"
 * Created by Ilias Lamprou
 * Updated 23 02 2016
 * This is the code you need to run on your arduino board to cummunicate whith VirtUino app via bluetooth
 * Before  running this code config the settings below as the instructions on the right
 * 
 * 
 * Download latest Virtuino android app from the link: https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 * Getting starting link:
 * Video tutorial link: https://www.youtube.com/watch?v=CYR_jigRkgk
 * Contact address for questions or comments: iliaslampr@gmail.com
 */

/*========= VirtuinoBluetooth Class methods  
*  vPinMode(int pin, int state)                                  set pin as digital OUTPUT or digital INPUT.  (Insteed default pinMode method
*
*========= Virtuino General methods  
*  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)   write a value to a Virtuino integer DV memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
*  int  vDigitalMemoryRead(int digitalMemoryIndex)               read  the value of a Virtuino integer DV memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
*  void vMemoryWrite(int analogMemoryIndex, float value)         write a value to Virtuino float V memory       (memoryIndex=0..31, value range as float value)
*  float vMemoryRead(int analogMemoryIndex)                      read the value of  Virtuino float V memory    (analogMemoryIndex=0..31, returned value range = 0..1023)
*  run()                                                         neccesary command to communicate with Virtuino android app  (on start of void loop)
*  void vDelay(long milliseconds);                               Pauses the program (without block communication) for the amount of time (in miliseconds) specified as parameter
*  int getPinValue(int pin)                                      read the value of a Pin. Usefull for PWM pins
*/




#include "VirtuinoBluetooth.h"               // Include VirtuinoBluetooth library to your code
 VirtuinoBluetooth virtuino(Serial1);        
                                                 

// Connect the HC-05 module to Arduino Mega - Due Serial1 port as the plan below
//  http://iliaslamprou.mysch.gr/virtuino/mega/virtuino_mega_hc05_connections.png

// Arduino Mega settings -> Open VirtuinoBluetooth.h file on the virtuino library folder ->  disable the line: #define BLUETOOTH_USE_SOFTWARE_SERIAL 

// Arduino Due settings  -> 1.Open VirtuinoBluetooth.h on the virtuino library folder ->  disable the line: #define BLUETOOTH_USE_SOFTWARE_SERIAL 
//                       -> 2.Open VirtuinoEsp8266_WebServer file on the virtuino library folder ->  disable the line: #define ESP8266_USE_SOFTWARE_SERIAL 

//================================================================== setup
//==================================================================
//==================================================================
void setup(){
  
  virtuino.DEBUG=true;               // set this value TRUE to enable the serial monitor status
  Serial.begin(9600);                // Set serial monitor baud rate

  Serial1.begin(9600);             // Set the hardware serial baud rate
  
  
  // Use virtuino.vPinMode instead default pinMode method for digital input or digital output pins.
  
  //pinMode(13,OUTPUT);
    virtuino.vPinMode(13,OUTPUT);                               // Don't use vPinMode for PWM pins, or pins that their values change many times per second
                                                                // Every time the value of a vPinMode  pin is changed the virtuino library sends a message and inform virtuino about the pin value.
                                                                // If an PWM pin is declared as vPinMode pin the arduino will continuously try  to send data to android device because the value of a pwm pin changes constantly  
   
//------ enter your setup code below
  

 
    
    
}


//================================================================== loop
//==================================================================
//==================================================================


void loop(){
   virtuino.run();           //  necessary command to communicate with Virtuino android app
   
    //------ enter your loop code below here
    //------ avoid to use delay() function in your code. Use the command virtuino.vDelay() instead of delay()
    //------ your code .....

    
   // in this "Hello Virtuino World" example you don't have to add any code 

     

 }


