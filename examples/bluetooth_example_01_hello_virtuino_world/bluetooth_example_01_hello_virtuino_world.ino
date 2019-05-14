/* Virtuino example No1 (Bluetooth)
 * Example name = "Hello Virtuino World"
 * Created by Ilias Lamprou
 * Updated Jul 01 2016
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
*  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)   write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
*  int  vDigitalMemoryRead(int digitalMemoryIndex)               read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
*  void vMemoryWrite(int analogMemoryIndex, float value)         write a value to Virtuino float memory       (memoryIndex=0..31, value range as float value)
*  float vMemoryRead(int analogMemoryIndex)                      read the value of  Virtuino analog memory    (analogMemoryIndex=0..31, returned value range = 0..1023)
*  run()                                                         neccesary command to communicate with Virtuino android app  (on start of void loop)
*  void vDelay(long milliseconds);                               Pauses the program (without block communication) for the amount of time (in miliseconds) specified as parameter
*  int getPinValue(int pin)                                      read the value of a Pin. Usefull for PWM pins
*/




#include "VirtuinoBluetooth.h"                           // Include VirtuinoBluetooth library to your code

// Code to use SoftwareSerial
#include <SoftwareSerial.h>                              //  Disable this line if you want to use hardware serial 
SoftwareSerial bluetoothSerial =  SoftwareSerial(2,3);   // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to bluetooth module TX pin   -  connect the arduino TX pin to bluetooth module RX pin.  Disable this line if you want to use hardware serial 
VirtuinoBluetooth virtuino(bluetoothSerial);       // Set SoftwareSerial baud rate.  -  Disable this line if you want to use hardware serial 


// Code to use HardwareSerial
// VirtuinoBluetooth virtuino(Serial1);            // enable this line and disable all SoftwareSerial lines
                                                   // Open VirtuinoBluetooth.h file on the virtuino library folder ->  disable the line: #define BLUETOOTH_USE_SOFTWARE_SERIAL 
                                                  


                                              




//================================================================== setup
//==================================================================
//==================================================================
void setup(){

  virtuino.DEBUG=true;               // set this value TRUE to enable the serial monitor status
  Serial.begin(9600);                // Set serial monitor baud rate
  
  bluetoothSerial.begin(9600);         // Disable this line if you want to use software serial (Mega, DUE etc.)
  //Serial1.begin(9600);               // Enable this line if you want to use hardware serial (Mega, DUE etc.)
  
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


