/*
 * IRremoteESP8266: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * This is very simple teaching code to show you how to use the library.
 * If you are trying to decode your Infra-Red remote(s) for later replay,
 * use the IRrecvDumpV2.ino example code instead of this.
 * An IR detector/demodulator must be connected to the input kRecvPin.
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 * Example circuit diagram:
 *  https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-receiving
 * Changes:
 *   Version 0.2 June, 2017
 *     Changed GPIO pin to the same as other examples.
 *     Used our own method for printing a uint64_t.
 *     Changed the baud rate to 115200.
 *   Version 0.1 Sept, 2015
 *     Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009
 */

#include <ESP8266WiFi.h>
#include <Arduino.h>``
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>


// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
const uint16_t kRecvPin = 14;



IRrecv irrecv(kRecvPin);            //nitialising to recieve signal from IR Reciever.

decode_results results;             //decoding results n result variable


const char* ssid = "abhyam";                            //SSID of my wifi router
const char* password = "abhyam@iit";                    //PASS of my wifi router

int LED = D0;                        // led connected to D0

WiFiServer server(80);               //intialises the server on port 80.
int t;                               //variable to store decoded value from IR Reciever(TSOP1738)
int i=0;                           //just a flag to get out of while loop
int j=1;                           //Just a flag to know the status of web interface.
int i1=0;                          //just a flag to get out of while loop
int surity=1;                      //Surity==0 means that the wifi is not connected for sure, and ==1 means the wifi is connected for sure but somehow the IR Mode maybe in use.


void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);



  irrecv.enableIRIn();  // Start the receiver
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);


  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED, HIGH);

  pinMode(5, OUTPUT);          //(D1 in NodeMCU)
  pinMode(4, OUTPUT);          //(D2 in NodeMCU)
  digitalWrite(4, HIGH);       //so that initially the lights will be switched off after turnng onn the module.
  digitalWrite(5, HIGH);       //so that initially the lights will be switched off after turnng onn the module.

  Serial.print("Connecting to the Newtork");
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED )                     //Turn onn the In-Built led connected to pin 5 , untill it runs
  {
    digitalWrite(LED,LOW);
    delay(500);
    i=i+1;
    if (i==20){                                              //waits for 10 seconds to get connected to wifi server
      j=0;                  //this indicated that the Node MCU is not connected to the wifi for now.
      surity=0;             //this indicated that the wifi is not there to connect for sure ,
                            //means if i press the button to go into wifi mode then it will not be possible.
      digitalWrite(LED, HIGH);                               //this idicated the LED on D0 shall remail off as it booiting into IR mode.
      break;
    }
    digitalWrite(LED, HIGH);
    Serial.print(".");
  }

  //if module gets connected to wifi within 10 second of turning on.
  if(j==1){
    Serial.println(" ");
    Serial.println("WiFi connected");
    server.begin();                                                 // Starts the Server
    Serial.println("Server started");

    Serial.print("IP Address of network: ");                        // Prints IP address on Serial Monitor
    Serial.println(WiFi.localIP());
    Serial.print("Copy and paste the following URL: https://");
    Serial.print(WiFi.localIP());
    Serial.println("/");

    while(i1<=60){                                                            //waits for 6 second to decide.
      digitalWrite(LED, LOW);
      if (irrecv.decode(&results)){
    //    print() & println() can't handle printing long longs. (uint64_t)
    //    serialPrintUint64(results.value, DEC);
    //    Serial.println("");
        t=results.value;
        if(t==16195807){
          j=0;                                  //means boots into only IR function, that means no web interface.
          Serial.println("now using the IR system");
          digitalWrite(LED, HIGH);              //the LED will remain off , indicating the use of IR Mode.
          break;
        }
        else if(t==16244767){
          j=1;                                  //means proccess only web interface, and no IR interactions
          Serial.println("Now the system is goint to use the only Web Interface");
          digitalWrite(LED, LOW);               //Now the LED will starts to glow, Means the Web interface is now in use.
          break;
        }
        irrecv.resume();                         // Receive the next value
      }
      Serial.print("!");
      delay(100);
      i1=i1+1;
      digitalWrite(LED, LOW);                       //LED is turned onn, means
    }
  }


  Serial.print("value of t: ");
  Serial.println(t);
  Serial.print("Value of j ");
  Serial.println(j);
  Serial.print("Value of surity: ");
  Serial.println(surity);


}

void loop()
{
  // Serial.print("Value of j ");
  // Serial.println(j);
  // Serial.print("Value of surity: ");
  // Serial.println(surity);

  if (j==1 && surity==1){

    WiFiClient client = server.available();

    if (!client)
    {
      return;
    }

    Serial.println("Waiting for new client");

    while(!client.available())
    {
      delay(1);
    }

    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    int value_4 = LOW;                      //Store the Stuatus of Relay 1 on pin 4.
    int value_5 = LOW;                      //Store the Stuatus of Relay 2 on pin 5.


    if(request.indexOf("/pin_4=ON") != -1)
    {
      digitalWrite(4, LOW); // Turn ON relay 1
      value_4 = HIGH;
    }
    if(request.indexOf("/pin_4=OFF") != -1)
    {
      digitalWrite(4, HIGH); // Turn OFF relay 1
      value_4 = LOW;
    }
    if(request.indexOf("/pin_5=ON") != -1)
    {
      digitalWrite(5, LOW); // Turn ON relay 2
      value_5 = HIGH;
    }
    if(request.indexOf("/pin_5=OFF") != -1)
    {
      digitalWrite(5, HIGH); // Turn ON relay 2
      value_5 = HIGH;
    }
    if( request.indexOf("/switch_option=yes") != -1){
      if (j==1){
        j=0;
      }

    }

  /*------------------HTML Page Creation---------------------*/

    client.println("HTTP/1.1 200 OK"); // standalone web server with an ESP8266
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");

    client.print("RELAY 1: ");

    if(value_4 == HIGH)
    {
      client.print("ON");
    }
    else
    {
      client.print("OFF");
    }

    client.println("<br><br>");
    client.println("<a href=\"/pin_4=ON\"\"><button>ON</button></a>");
    client.println("<a href=\"/pin_4=OFF\"\"><button>OFF</button></a><br />");


    client.println("<br><br><hr><br><br>");

    client.print("RELAY 2: ");

    if(value_5 == HIGH){
      client.print("ON");
    }
    else{
      client.print("OFF");
    }

    client.println("<br><br>");
    client.println("<a href=\"/pin_5=ON\"\"><button>ON</button></a>");
    client.println("<a href=\"/pin_5=OFF\"\"><button>OFF</button></a><br />");
    client.println("<br><br>");


    client.println("<entre> SWITCH TO IR MODE </centre>");
    client.println("<centre><a href=\"/switch_option=yes\"\"><button>Press this to go to IR MODE</button></a></centre>");
    client.println("</html>");

    delay(1);
    Serial.println("Client disonnected");
    Serial.println("");
  }

  else{
    Serial.println("Can't able to use web interface inside void loop");
    j=0;                                                    //This indicated that the web interface still can't be used.
    digitalWrite(LED,HIGH);                                 //LED will remain off indicating still use of IR interface
  }





 if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, DEC);
    Serial.println("");
    t=results.value;
    if(t==16203967){
//      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(5, HIGH);
    }
    else if (t==16236607){
//      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(5, LOW);
    }
    else if(t==16220287){
      digitalWrite(4, LOW);
    }
    else if(t==16187647){
      digitalWrite(4, HIGH);
    }
    else if(t==16244767){
      if(WiFi.status() == WL_CONNECTED){                        //checks if the module is connected after some time when router is turned on later
          surity=1;
          delay(300);
          server.begin();                                       // Starts the Server
          Serial.println("Server started");
          delay(200);
        }

      j=1;
      Serial.println("Now trying to use the web interface");
      digitalWrite(LED, LOW);                                   //LED will turn on indiacting use of web interface
    }
    irrecv.resume();    // Receive the next value
  }
  delay(100);
}
