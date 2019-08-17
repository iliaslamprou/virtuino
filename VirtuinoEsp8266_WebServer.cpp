 /* Virtuino ESP8266 web server Library
 * Created by Ilias Lamprou
 * Updated Aug 17 2019
 * 
 * Download latest Virtuino android app from the link:  https://play.google.com/store/apps/details?id=com.virtuino_automations.virtuino
 * Contact address for questions or comments: iliaslampr@gmail.com
 */




//========= VirtuinoEsp8266 Class methods  
// boolean connectESP8266_toInternet(String wifiNetworkName,String wifiNetworkPassword, int port);  Set your home wifi network SSID and PASSWORD  (Put this function on start of void setup)

// boolean createLocalESP8266_wifiServer(String wifiNetworkName,String wifiNetworkPassword, int port, int mode); Use this function to create an ESP8266 wifi local network
//                                                                    set port to 80 
//                                                                    set mode=2 to use ESP8266 only as access point 
//                                                                    set mode=3 to use ESP8266 as access point and internet station.
//========= Virtuino general methods  
//  void vDigitalMemoryWrite(int digitalMemoryIndex, int value)       write a value to a Virtuino digital memory   (digitalMemoryIndex=0..31, value range = 0 or 1)
//  int  vDigitalMemoryRead(int digitalMemoryIndex)                   read  the value of a Virtuino digital memory (digitalMemoryIndex=0..31, returned value range = 0 or 1)
//  void vMemoryWrite(int memoryIndex, float value);                  write a value to Virtuino memory             (memoryIndex=0..31, value range as float value)
//  float vMemoryRead(int memoryIndex);                               read a value of  Virtuino memory             (memoryIndex=0..31, returned a float value
//  run();                                                            neccesary command to communicate with Virtuino android app  (on start of void loop)
//  int getPinValue(int pin);                                         read the value of a Pin. Usefull to read the value of a PWM pin
//  void clearTextBuffer();                                           Clear the text received text buffer
//  int textAvailable();                                              Check if there is text in the received buffer
//  String getText(byte ID);                                          Read the text from Virtuino app
//  void sendText(byte ID, String text);                              Send text to Virtuino app  



#include "VirtuinoEsp8266_WebServer.h"





//======================================================================================== VirtuinoEsp8266_WebServer init
//========================================================================================
#ifdef ESP8266_USE_SOFTWARE_SERIAL
VirtuinoEsp8266_WebServer::VirtuinoEsp8266_WebServer(SoftwareSerial &uart): espSerial(&uart){}
VirtuinoEsp8266_WebServer::VirtuinoEsp8266_WebServer(SoftwareSerial &uart, uint32_t baud): espSerial(&uart){
    espSerial->begin(baud);
    while (espSerial->available()) espSerial->read();
}


#else
VirtuinoEsp8266_WebServer::VirtuinoEsp8266_WebServer(HardwareSerial &uart, uint32_t baud): espSerial(&uart){
    espSerial->begin(baud);
    while (espSerial->available()) espSerial->read();
}
VirtuinoEsp8266_WebServer::VirtuinoEsp8266_WebServer(HardwareSerial &uart): espSerial(&uart){}
#endif


//======================================================================================== run
//========================================================================================
 void  VirtuinoEsp8266_WebServer::run() {
    if(espSerial->available()){
       while(!espSerial->available()) delay(1);
       espBuffer = espSerial->readStringUntil('\r');
          if (DEBUG) Serial.print(espBuffer);
          int pos = espBuffer.indexOf("+IPD,");
          if (pos>=0){                                        // check for GET command from Virtuino app
              clearESP_buffer(100);
              int connectionId = espBuffer.charAt(pos+5)-48;  // get connection ID
              if (DEBUG) Serial.println("ID="+ String(connectionId));
              pos = espBuffer.indexOf("GET /");
              if (pos!=-1){                                   // We have a GET message
                  espBuffer.remove(0,pos+5);
                  checkVirtuinoCommand(&espBuffer);
                  boolean b=wifiSendData(connectionId,&espResponseBuffer);
                  espResponseBuffer="";
                  clearESP_buffer(50);
             } 
          } 
        espBuffer="";                                     // clear buffer for the next line
   } //if 
  
}

//======================================================================================== connectESP8266_toInternet
//======================================================================================== 
void VirtuinoEsp8266_WebServer::connectESP8266_toInternet(String wifiNetworkName,String wifiNetworkPassword, int port){
    clearESP_buffer(1000);
    if (DEBUG) Serial.println( "Server init...");
    espSerial->println("AT+GMR");
    waitForResponse("OK",1000);
    
    espSerial->println("AT+CWMODE=1");  // configure as client
    waitForResponse("OK",1000);
    
    //--- connect
    espSerial->print("AT+CWJAP=\"");
    espSerial->print(wifiNetworkName);
    espSerial->print("\",\"");
    espSerial->print(wifiNetworkPassword);
    espSerial->println("\"");
    waitForResponse("OK",10000);
     
    espSerial->println("AT+CIFSR");           // get ip address
    waitForResponse("OK",1000);
    
    espSerial->println("AT+CIPMUX=1");         // configure for multiple connections   
    waitForResponse("OK",1000);
    
    espSerial->print("AT+CIPSERVER=1,");
    espSerial->println(port);
    waitForResponse("OK",1000);
   
}
//======================================================================================== createLocalESP8266_wifiServer
//========================================================================================
void  VirtuinoEsp8266_WebServer::createLocalESP8266_wifiServer(String wifiNetworkName,String wifiNetworkPassword, int port, int mode){
      if (DEBUG) Serial.println( "Server init...");
      //if ((mode<2)|| (mode>3)) mode=2; 
      
      clearESP_buffer(1000);
      espSerial->println("AT+GMR");
      waitForResponse("OK",1000);

      espSerial->println("AT+CWMODE=2");  // configure as client
      waitForResponse("OK",1000);

     espSerial->println("AT+CIFSR");
     waitForResponse("OK",1000);

     espSerial->print("AT+CWSAP=\"");
     espSerial->print(wifiNetworkName);
     espSerial->print("\",\"");
     espSerial->print(wifiNetworkPassword);
     espSerial->println("\",2,3");
     waitForResponse("OK",5000);
     
     espSerial->println("AT+CIPMUX=1");         // configure for multiple connections   
     waitForResponse("OK",2000);
    
     espSerial->print("AT+CIPSERVER=1,");
     espSerial->println(port);
     waitForResponse("OK",1000);           
}

//========================================================================================
boolean VirtuinoEsp8266_WebServer::esp8266_setIP(byte a1, byte a2, byte a3, byte a4){
    clearESP_buffer(100);    // clear esp buffer
    espSerial->println("AT+CIPSTA=\""+String(a1)+"."+String(a2)+"."+String(a3)+"."+String(a4)+"\"");
    return waitForResponse("OK",3000);
}

//====================================================================================== checkVirtuinoCommand
//======================================================================================
void  VirtuinoEsp8266_WebServer::checkVirtuinoCommand(String* command){
  int pos=command->lastIndexOf(wf_COMMAND_END_CHAR);                      
  if (pos>5){                                          // check the size of command
      command->remove(pos+1);     // clear the end of command buffer
     //------------------  get command password
      String commandPassword="";
      int k=command->indexOf(wf_COMMAND_START_CHAR);
       if (k>0) {
        commandPassword = command->substring(0,k);
        command->remove(0,k);   // clear the start part of the command  buffer
       }
    
      espResponseBuffer="";
      if ((password.length()==0) || (commandPassword.equals(password))) {                       // check if password correct
           while (command->indexOf(wf_COMMAND_START_CHAR)>=0){
              int cStart=command->indexOf(wf_COMMAND_START_CHAR);
              int cEnd=command->indexOf(wf_COMMAND_END_CHAR);
              if (cEnd-cStart>5){
                String oneCommand = command->substring(cStart+1,cEnd);                               // get one command
                char commandType = getCommandType(oneCommand.charAt(0));
                  if (commandType!='E') {
                     int pin= getCommandPin(&oneCommand);
                     if (pin!=-1){
                        boolean returnInfo=false;
                        if (oneCommand.charAt(4)=='?') returnInfo=true;
                        oneCommand.remove(0,4);
                        if (oneCommand.length()<wf_TEXT_COMMAND_MAX_SIZE) executeReceivedCommand(commandType,pin,&oneCommand,returnInfo);
                        else {
                          espResponseBuffer+=getErrorCommand(wf_ERROR_SIZE);
                          String s="";
                          if (pin<9) s+="0";
                          s+=String(pin);
                          espResponseBuffer+=wf_COMMAND_START_CHAR;
                          espResponseBuffer+='T';
                          espResponseBuffer+=s;
                          espResponseBuffer+="= ";
                          espResponseBuffer+=wf_COMMAND_END_CHAR;
                          
                        }
                        lastCommunicationTime=millis();
                      //  espResponseBuffer += commandResponse;
                     } else  espResponseBuffer +=getErrorCommand(wf_ERROR_PIN);  // response  error pin number   !E00=1$   
                  } else espResponseBuffer +=getErrorCommand(wf_ERROR_TYPE);  // response  error type   !E00=3$   
          
              } else{
                espResponseBuffer+=getErrorCommand(wf_ERROR_SIZE);  // response  error size of command   !E00=4$   
              }
              command->remove(0,cEnd+1-cStart);
           }  // while
      } else espResponseBuffer+=getErrorCommand(wf_ERROR_PASSWORD);     // the password is not correct
  }
  else  espResponseBuffer+=getErrorCommand(wf_ERROR_SIZE);         // the size of command is not correct
 
}

//======================================================================================== waitForResponse
//========================================================================================
boolean VirtuinoEsp8266_WebServer::waitForResponse(String target1,  int timeout){
    String data;
    char a;
    unsigned long start = millis();
    boolean rValue=false;
    while (millis() - start < timeout) {
        while(espSerial->available() > 0) {
            a = espSerial->read();
            if (DEBUG) Serial.print(a);
            if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target1) != -1) {
            rValue=true;
            break;
        } 
    }
    return rValue;
}

//========================================================================================  wifiSendData
//========================================================================================
boolean VirtuinoEsp8266_WebServer::wifiSendData(int connectionId, String* content){
  boolean returnedValue=false;
  // build HTTP response
 // HTTP Header
  String httpResponse = "HTTP/1.1 200 OK\r\n";
  httpResponse += "Content-Type: text/html; charset=UTF-8\r\n";
  httpResponse += "Content-Length: ";
  httpResponse += content->length();
  httpResponse += "\r\n";
  httpResponse += "Connection: close\r\n\r\n";
  
  //if (DEBUG) Serial.println("httpResponse="+httpResponse);
  
  httpResponse += *content;
  httpResponse += (" ");
   //---Send command
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += httpResponse.length();
  cipSend += "\r\n";

  for (int i=0;i<cipSend.length();i++) espSerial->write(cipSend.charAt(i));

  if (waitForResponse(">",2000)) {
      for (int i=0;i<httpResponse.length();i++) {
         espSerial->write(httpResponse.charAt(i));
         if (DEBUG) Serial.print(httpResponse.charAt(i));
       }

      if (waitForResponse("SEND OK",10000)) returnedValue=true;
  }
  return returnedValue;
}





//========================================================================================  sendCommand
//========================================================================================
  String VirtuinoEsp8266_WebServer::sendCommand(String command, const int timeout){
  String response = "";
  espSerial->print(command); // send the read character to the esp8266
  long int time = millis();
  while ( (time + timeout) > millis()){
    while (espSerial->available()){
      char c = espSerial->read(); // read the next character.
      response += c;
    }
  }
if (DEBUG) Serial.print(response);
  return response;
}

//========================================================================================  closeClientConnection
//========================================================================================
void VirtuinoEsp8266_WebServer::closeClientConnection(int connectionID){
    String closeCommand = "AT+CIPCLOSE=";       // close AT command 
    closeCommand+=connectionID;                 // append connection id
    closeCommand+="\r\n";
    espSerial->print(closeCommand); 
  
}

//======================================================================================== clearESP_buffer
//========================================================================================
void VirtuinoEsp8266_WebServer::clearESP_buffer(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (espSerial->available()){
        c=espSerial->read();
        if (DEBUG) Serial.print(c);
      }
    }
   }
   //======================================================================================== getCommandType
   //========================================================================================
  //This function returns the command type which was received from app
  //The second character of command string determines the command type
  // I  digital input read command    
  // Q  digital ouput write
  // D  digital memory read - write commang
  // A  analog input analog input read command
  // O  analog output write command
  // V  float memory read - write command
  // C  connect or disconect command  
  // T  text command  
  // E  error
  
  // Other characters are recognized as an error and function returns the character E
  //This is a system fuction. Don't change this code
  
  char  VirtuinoEsp8266_WebServer::getCommandType(char c){
    if (!(c== 'I' || c == 'Q' || c == 'D' ||c == 'A' ||c == 'O' || c == 'V' || c == 'C' || c == 'T')){
      c='E'; //error  command
    }
    return c;
  }

  //======================================================================================== getCommandPin
  //========================================================================================
  //This function returns the pin number of the command string which was received from app
  //Fuction checks if pin number is correct. If not it returns -1 as error code
  //This is a system fuction. Don't change this code
 
  int  VirtuinoEsp8266_WebServer::getCommandPin(String* aCommand){
    char p1= aCommand->charAt(1);
    char p2= aCommand->charAt(2);
    String s="-1";
    if (isDigit(p1) && isDigit(p2)) {
       s="";
       s+=p1;
       s+=p2;
    }
     return s.toInt();
  }

//======================================================================================== getCommandValue
//========================================================================================
float VirtuinoEsp8266_WebServer::getCommandValue(String* aCommand){
     boolean er=false;
     for (int i=0;i<aCommand->length();i++){
        char c=aCommand->charAt(i);
        if (! ((c=='.') || (c=='-') || (isDigit(aCommand->charAt(i))))) er=true;
       }
      if (er==false) return aCommand->toFloat();
    
    return 0;
  }

//=========================================================================================
//=========================================================================================
String VirtuinoEsp8266_WebServer::getErrorCommand(byte code){
  String rValue="";
  rValue+=wf_COMMAND_START_CHAR;
  rValue+="E00=";
  rValue+=String(code);
  rValue+=wf_COMMAND_END_CHAR;
  return rValue;
}
//================================================================================== urlencode
//==================================================================================
String VirtuinoEsp8266_WebServer::urlencode(String* str){
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str->length(); i++){
      c=str->charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}

//================================================================================== h2int
//==================================================================================
unsigned char VirtuinoEsp8266_WebServer::h2int(char c){
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}
//================================================================================== urldecode
//==================================================================================
String VirtuinoEsp8266_WebServer::urldecode(String* str){
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str->length(); i++){
        c=str->charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str->charAt(i);
        i++;
        code1=str->charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
         encodedString+=c;  
      }
      yield();
    }
    
   return encodedString;
}

//======================================================================================== executeCommandFromApp
//========================================================================================
  void VirtuinoEsp8266_WebServer::executeReceivedCommand(char activeCommandType, int activeCommandPin ,String* commandString, boolean returnInfo){
    //------------------------------------
   // Every time we have a complete command from bluetooth the global value commandState is set to 1 
   // The value activeCommandType contains command type such as Digital output, Analog output etc.
   // The value activeCommandPin contains the pin number of the command
   // The value activeCommandValue contains the value (0 or 1 for digital, 0-255 for analog outputs, 0-1023 for analog memory) 
   // In this void we have to check if the command is correct and then execute the command 
   // After executing the command we have to send a response to app
   // At last we must reset the command state value to zero 
   //This is a system fuction. Don't change this code
      
    String response=getErrorCommand(wf_ERROR_UNKNOWN); 
    String pinString="";
    if (activeCommandPin<10) pinString = "0"+String(activeCommandPin);
    else pinString=String(activeCommandPin);
    float activeCommandValue=0;
     switch (activeCommandType) {
      case 'T':                         
             if ((activeCommandPin>=0) & (activeCommandPin < 100)){
                if (returnInfo) {
                  if (textRequestedHandler==NULL) return;
                  String testReply = textRequestedHandler(activeCommandPin);
                  if (testReply==NO_REPLY) return;
                  response="";
                  response =wf_COMMAND_START_CHAR;
                  response +=activeCommandType;
                  response +=pinString;
                  response +="=";
                  response += urlencode(&testReply);
                  response +=wf_COMMAND_END_CHAR;  // response 
                }
                else {
                  if (textReceivedHandler!=NULL) {
                    String decodeStr = urldecode(commandString);
                    textReceivedHandler(activeCommandPin,decodeStr);
                  }
                }
            }
          break;
      case 'I':                         
            if ((activeCommandPin>=0) & (activeCommandPin < wf_arduinoPinsSize))
              response =wf_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(digitalRead(activeCommandPin))+wf_COMMAND_END_CHAR;  // response 
            else   response =getErrorCommand(wf_ERROR_PIN);  // response  error pin number   !E00=1$       
          break;
      case 'Q': 
            if ((activeCommandPin>=0) & (activeCommandPin < wf_arduinoPinsSize)){
                 if (returnInfo) response =wf_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(digitalRead(activeCommandPin))+wf_COMMAND_END_CHAR;  // response 
                 else {
                   activeCommandValue = getCommandValue(commandString);
                   if ((activeCommandValue == 0) || (activeCommandValue == 1)) {
                          digitalWrite(activeCommandPin,activeCommandValue);
                          arduinoPinsValue[activeCommandPin]=activeCommandValue;
                          response =wf_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(digitalRead(activeCommandPin))+wf_COMMAND_END_CHAR;  // response 
                   } else    response =getErrorCommand(wf_ERROR_VALUE);
                   }
            } else   response =getErrorCommand(wf_ERROR_PIN);;  // response  error pin number   !E00=1$   
          break; 

           

       case 'D':
            if ((activeCommandPin>=0) & (activeCommandPin<wf_virtualDigitalMemorySize)){ 
                if (returnInfo) response =wf_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(virtualDigitalMemory[activeCommandPin])+wf_COMMAND_END_CHAR;  // response 
                else{
                      activeCommandValue = getCommandValue(commandString);
                      virtualDigitalMemory[activeCommandPin]= activeCommandValue; 
                      response =wf_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(virtualDigitalMemory[activeCommandPin])+wf_COMMAND_END_CHAR;  // response 
                }// else    response =getErrorCommand(wf_ERROR_VALUE);
            } else   response =getErrorCommand(wf_ERROR_PIN);  // response  error pin number   !E00=1$   
            break; 
      
       case 'V': 
           if ((activeCommandPin>=0) & (activeCommandPin<wf_virtualAnalogMemorySize)){
               if (returnInfo) response =wf_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(virtualFloatMemory[activeCommandPin])+wf_COMMAND_END_CHAR;  // response
               else { 
                    activeCommandValue = getCommandValue(commandString);
                    virtualFloatMemory[activeCommandPin]= activeCommandValue; 
                    response =wf_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(virtualFloatMemory[activeCommandPin])+wf_COMMAND_END_CHAR;  // response
                    } 
           } else   response =getErrorCommand(wf_ERROR_PIN);
          break;
       case 'O': 
           if ((activeCommandPin>=0) & (activeCommandPin < wf_arduinoPinsSize)){
                   
                   if (returnInfo) {
                      int pwm_value = pulseIn(activeCommandPin, HIGH);
                      pwm_value = pwm_value /7.85;
                      response =wf_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String(pwm_value)+wf_COMMAND_END_CHAR;  // response 
                  }
                   else {
                    activeCommandValue = getCommandValue(commandString); 
                          
                          arduinoPinsValue[activeCommandPin]=(int) activeCommandValue;
                          analogWrite(activeCommandPin,(int)activeCommandValue);
                          response =wf_COMMAND_START_CHAR+String(activeCommandType)+pinString+"="+String((int)activeCommandValue)+wf_COMMAND_END_CHAR;  // response 
                   }
                        
           } else   response =getErrorCommand(wf_ERROR_PIN);;
          break;

         case 'A':                        
            if ((activeCommandPin>=0) & (activeCommandPin < arduinoAnalogPinsSize))
              response ="!"+String(activeCommandType)+pinString+"="+String(analogRead(analogInputPinsMap[activeCommandPin]))+"$";  // response 
            else   response =getErrorCommand(wf_ERROR_PIN);  // response  error pin number   !E00=1$       
          break;  

          case 'C':  
                      response="";  
                       activeCommandValue= getCommandValue(commandString);
                      if (activeCommandValue==1)response =wf_firmwareCode;//"!C00="+String(wf_firmwareCode)"+"$";//wf_COMMAND_START_CHAR+"C"+pinString+"=1"+wf_COMMAND_END_CHAR;     // return firmware version
                      break;
           }
        espResponseBuffer += response;
      
  }

//=================================================================== getPinValue
int VirtuinoEsp8266_WebServer::getPinValue(int pin){
  if (pin>=0 && pin<wf_arduinoPinsSize) return arduinoPinsValue[pin];
  else return 0;
  }
  //=================================================================== vmDigitalWrite
 void VirtuinoEsp8266_WebServer::vDigitalMemoryWrite(int digitalMemoryIndex, int value){
  if ((digitalMemoryIndex>=0) && (digitalMemoryIndex<wf_virtualDigitalMemorySize)){
    virtualDigitalMemory[digitalMemoryIndex]=value;
  }
  
}
//=================================================================== vmDigitalRead
 int VirtuinoEsp8266_WebServer::vDigitalMemoryRead(int digitalMemoryIndex){
  if ((digitalMemoryIndex>=0) && (digitalMemoryIndex<wf_virtualDigitalMemorySize)){
    return virtualDigitalMemory[digitalMemoryIndex];
  }
  else return 0;    // error
}

//=================================================================== vmAnalogWrite
void VirtuinoEsp8266_WebServer::vMemoryWrite(int memoryIndex, float value){
  if ((memoryIndex>=0) && (memoryIndex<wf_virtualAnalogMemorySize)){
      virtualFloatMemory[memoryIndex]=value;
  }
}

//=================================================================== vmAnalogRead
 float VirtuinoEsp8266_WebServer::vMemoryRead(int memoryIndex){
  if ((memoryIndex>=0) && (memoryIndex<wf_virtualAnalogMemorySize)){
    return virtualFloatMemory[memoryIndex];
  }
  else return 0;    // error
}  

//===================================================================
void VirtuinoEsp8266_WebServer::vDelay(long milliseconds){
  long timeStart=millis();
  while (millis()-timeStart<milliseconds) run();
}
