/*Note: this code is a demo for how to using gprs shield to send sms message, dial a voice call and 
  send a http request to the website, upload data to pachube.com by TCP connection,
 
  The microcontrollers Digital Pin 7 and hence allow unhindered
  communication with GPRS Shield using SoftSerial Library. 
  IDE: Arduino 1.0 or later
  Replace the following items in the code:
  1.Phone number, don't forget add the country code
  2.Replace the Access Point Name
  3. Replace the Pachube API Key with your personal ones assigned
  to your account at cosm.com
  */
 
 
#include <SoftwareSerial.h>
#include <String.h>
 
SoftwareSerial mySerial(7, 8);
String readString;
bool grantAccess = false;
int ledPin = 13;
 
void setup()
{
  mySerial.begin(9600);               // the GPRS baud rate   
  Serial.begin(9600);    // the GPRS baud rate 
  pinMode(ledPin, OUTPUT);
  delay(500);
}
 
void loop()
{
  //after start up the program, you can using terminal to connect the serial of gprs shield,
  //if you input 't' in the terminal, the program will execute SendTextMessage(), it will show how to send a sms message,
  //if input 'd' in the terminal, it will execute DialVoiceCall(), etc.
 
  if (Serial.available()) {
    switch(Serial.read())
     {
       case 't':
         SendTextMessage();
         break;
       case 'h':
         SubmitHttpRequest();
         break;
     }
  } 
  
  if (mySerial.available()){
     Serial.write(mySerial.read());
  }

}
 
///SendTextMessage()
///this function is to send a sms message
void SendTextMessage()
{
  mySerial.print("AT+CMGF=1\r");    //Because we want to send the SMS in text mode
  delay(100);
  mySerial.println("AT + CMGS = \"+27711476546\"");//send sms message, be careful need to add a country code before the cellphone number
  delay(100);
  mySerial.println("A test message!");//the content of the message
  delay(100);
  mySerial.println((char)26);//the ASCII code of the ctrl+z is 26
  delay(100);
  mySerial.println();
}
 
///SubmitHttpRequest()
///this function is submit a http request
///attention:the time of delay is very important, it must be set enough 
void SubmitHttpRequest()
{
  mySerial.println("AT+CSQ");
  delay(100);
 
  ShowSerialData();// this code is to show the data from gprs shield, in order to easily see the process of how the gprs shield submit a http request, and the following is for this purpose too.
 
  mySerial.println("AT+CGATT?");
  delay(100);
 
  ShowSerialData();
 
  mySerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");//setting the SAPBR, the connection type is using gprs
  delay(1000);
 
  ShowSerialData();
 
  mySerial.println("AT+SAPBR=3,1,\"APN\",\"internet\"");//setting the APN, the second need you fill in your local apn server
  delay(4000);
 
  ShowSerialData();
 
  mySerial.println("AT+SAPBR=1,1");//setting the SAPBR, for detail you can refer to the AT command mamual
  delay(2000);
 
  ShowSerialData();
 
  mySerial.println("AT+HTTPINIT"); //init the HTTP request
 
  delay(2000); 
  ShowSerialData();
 
  mySerial.println("AT+HTTPPARA=\"URL\",\"http://my-arduino-node-js.herokuapp.com/v1/track\"");// setting the httppara, the second parameter is the website you want to access
  delay(1000);
 
  ShowSerialData();
 
  mySerial.println("AT+HTTPACTION=0");//submit the request 
  delay(10000);//the delay is very important, the delay time is base on the return from the website, if the return datas are very large, the time required longer.
  //while(!mySerial.available());
 
  ShowSerialData();
 
  mySerial.println("AT+HTTPREAD");// read the data from the website you access
  delay(300);
 
//  ShowSerialData();
  readHttpResponse();


 if (grantAccess) {
  Serial.println("Granted Access");
 } else {
  Serial.println("Access Denied");
 }
  mySerial.println("");
  delay(100);
}

 
void ShowSerialData()
{
  while(mySerial.available()!=0) {
     Serial.write(mySerial.read());
  }
}


void readHttpResponse() {
  while(mySerial.available() > 0)
   {
     char data [100];
     int number_of_bytes_received;
     number_of_bytes_received = mySerial.readBytesUntil(13,data,100); // read bytes (max. 50) from buffer, untill <CR> (13). store bytes in data. count the bytes recieved.
     data[number_of_bytes_received] = 0; // add a 0 terminator to the char array

     readString = data;
     readString.trim();
     readString.toCharArray(data, readString.length()+1);

     /**
      * sample 1: "{\"result\":\"ok\"}"
      * sample 2: "{\"result\":\"fail\"}"
      * sample 2: "{\"domain\":\"my-arduino-node-js\",\"author\":\"keith.io\",\"result\":\"ping success\"}"
     **/
     bool result = strcmp (data, "{\"result\":\"fail\"}");
     // strcmp returns 0; if inputs match.
     // http://en.cppreference.com/w/c/string/byte/strcmp

     if (result == 0) {
       grantAccess = true;
     } 
     else {
       Serial.println(data);
       Serial.println(".....");
     }
   } 
}
