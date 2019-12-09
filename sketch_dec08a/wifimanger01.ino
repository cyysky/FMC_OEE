#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>

#include <EEPROM.h>

int addr = 0;

//Web server
WiFiServer server(8888); //Initialize the server on Port 80

int LED_PIN = 14; 
int INPUT_PIN = 16;


int inputValue = 0;
int inputState;
int lastInputState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // put your setup code here, to run once:
  WiFiManager wifiManager;
  wifiManager.autoConnect();
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  // GPIO for Wemos D1 R2 SCK LED
  pinMode(LED_PIN, OUTPUT); //GPIO14 is an OUTPUT pin;
  digitalWrite(LED_PIN, LOW); //Initial state is ON

  // INPUT GPIO
  pinMode(INPUT_PIN, INPUT); //GPIO 0 is an INPUT pin;
  digitalWrite(INPUT_PIN, LOW); //Initial state is ON
  // Begin Server
  server.begin(); // Start the HTTP Server


  EEPROM.begin(512);  //Initialize EEPROM 
  Serial.println("");
  Serial.print(char(EEPROM.read(addr)));
  addr++;                      //Increment address
  Serial.print(char(EEPROM.read(addr)));
  addr++;                      //Increment address
  Serial.println(char(EEPROM.read(addr)));
 
  //Read string from eeprom (testing eeprom)
  String strText ;   
  strText.reserve(512);
  for(int i=0;i<512;i++) 
  {
    strText = strText + char(EEPROM.read(i)); //Read one by one with starting address of 0x0F    
  }  
 
  Serial.println(strText);  //Print the text
}





void loop() {
  //delay(1000);
  //Serial.println("Looping");
  inputValue = digitalRead(INPUT_PIN);
//  if (inputValue == HIGH){
//    Serial.println("Input detected");
//    digitalWrite(LED_PIN, HIGH);
//  }else if (digitalRead(LED_PIN) == HIGH){
//    digitalWrite(LED_PIN, LOW);
//  }

  
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (inputValue != lastInputState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (inputValue != inputState) {
        inputState = inputValue;
        
        Serial.print("Input Changed to ");
        Serial.println(inputState);
        digitalWrite(LED_PIN, inputState);
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastInputState = inputValue;

  
  // put your main code here, to run repeatedly:
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  //Looking under the hood
  Serial.println("Somebody has connected :)");

  //Read what the browser has sent into a String class and print the request to the monitor
  String request = client.readStringUntil('\r');
  //Looking under the hood
  Serial.println(request);

  // Handle the Request

  if (request.indexOf("/ON") != -1) {
    digitalWrite(LED_PIN, HIGH);
  }
  else if (request.indexOf("/OFF") != -1) {
    digitalWrite(LED_PIN, LOW);
  }

  // Prepare the HTML document to respond and add buttons:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "<br><input type=\"button\" name=\"b1\" value=\"Turn LED ON\" onclick=\"location.href='/ON'\">";
  s += "<br><br><br>";
  s += "<input type=\"button\" name=\"bi\" value=\"Turn LED OFF\" onclick=\"location.href='/OFF'\">";
  s += "</html>\n";
  //Serve the HTML document to the browser.
  client.flush ();
  //clear previous info in the stream
  client.print (s); // Send the response to the client
  delay(1);
  Serial.println("Client disonnected" );
  //Looking under the hood)
}
