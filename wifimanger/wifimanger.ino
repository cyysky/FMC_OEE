#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiClient.h>
#include <WiFiManager.h>

#include <EEPROM.h>

#include "index.h" //Our HTML webpage contents

int addr = 0;

//Web server
//WiFiServer server(8888); //Initialize the server on Port 80
ESP8266WebServer server2(88);

// GPIO Pin
int LED_PIN = 14; 
int INPUT_PIN = 16;

// Input Status and debouncing
int inputValue = 0;
int inputState;
int lastInputState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"=====(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    input1: <input type="text" name="counter">
    <input type="submit" value="Submit">
  </form><br>
</body></html>)=====";

const char* PARAM_INPUT_1 = "counter";
const char* PARAM_INPUT_2 = "input2";

//void notFound(AsyncWebServerRequest *request) {
//  request->send(404, "text/plain", "Not found");
//}

const String postForms = "<html>\
  <head>\
    <title>ESP8266 Web Server POST handling</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>POST plain text to /postplain/</h1><br>\
    <form method=\"post\" enctype=\"text/plain\" action=\"/postplain/\">\
      <input type=\"text\" name=\'{\"hello\": \"world\", \"trash\": \"\' value=\'\"}\'><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
    <h1>POST form data to /postform/</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
      <input type=\"text\" name=\"hello\" value=\"world\"><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  </body>\
</html>";


void setup() {
  // Turn on Serial at 115200 bound speed
  Serial.begin(115200);
  
  /*											*
  *				WIFI Manganger 		
  *												*/		
  
  // WiFi Manager 
  WiFiManager wifiManager;
  wifiManager.autoConnect();
  
  // If you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  
   /*											*
  *				GPIO Initialize 		
  *												*/		
  
  // GPIO for Wemos D1 R2 SCK LED
  pinMode(LED_PIN, OUTPUT); //GPIO14 is an OUTPUT pin;
  digitalWrite(LED_PIN, LOW); //Initial state is ON

  // INPUT GPIO
  pinMode(INPUT_PIN, INPUT); //GPIO 0 is an INPUT pin;
  digitalWrite(INPUT_PIN, LOW); //Initial state is ON
  
  
  
  /*											*
  *				EEPROM Initialize 		
  *												*/		 

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
  
  
//  /*											*
//  *				Web Server on port 8888 		
//  *												*/		
server2.on("/", handleRoot); //Which routine to handle at root location
server2.begin();
//  
//   // Send web page with input fields to client
//  server2.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send_P(200, "text/html", index_html);
//  });
//
//  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
//  server2.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
//    String inputMessage;
//    String inputParam;
//    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
//    if (request->hasParam(PARAM_INPUT_1)) {
//      inputMessage = request->getParam(PARAM_INPUT_1)->value();
//      inputParam = PARAM_INPUT_1;
//    }
//    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
//    else if (request->hasParam(PARAM_INPUT_2)) {
//      inputMessage = request->getParam(PARAM_INPUT_2)->value();
//      inputParam = PARAM_INPUT_2;
//    }
//    else {
//      inputMessage = "No message sent";
//      inputParam = "none";
//    }
//    Serial.println(inputMessage);
//    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
//                                     + inputParam + ") with value: " + inputMessage +
//                                     "<br><a href=\"/\">Return to Home Page</a>");
//  });
//  server2.onNotFound(notFound);
//  server2.begin(); // Start the HTTP Server
//  
  // Begin Server
  //server.begin(); // Start the HTTP Server

}

void handleRoot() {
  //String s = MAIN_page;
  server2.send(200, "text/html", index_html);
}



void loop() {

  server2.handleClient();          //Handle client requests
  
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
//  WiFiClient client = server.available();
//  if (!client) {
//    return;
//  }
//
//  //Looking under the hood
//  Serial.println("Somebody has connected :)");
//
//  //Read what the browser has sent into a String class and print the request to the monitor
//  String request = client.readStringUntil('\r');
//  //Looking under the hood
//  Serial.println(request);
//
//  // Handle the Request
//
//  if (request.indexOf("/ON") != -1) {
//    digitalWrite(LED_PIN, HIGH);
//  }
//  else if (request.indexOf("/OFF") != -1) {
//    digitalWrite(LED_PIN, LOW);
//  }
//
//  // Prepare the HTML document to respond and add buttons:
//  String s = "HTTP/1.1 200 OK\r\n";
//  s += "Content-Type: text/html\r\n\r\n";
//  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
//  s += "<br><input type=\"button\" name=\"b1\" value=\"Turn LED ON\" onclick=\"location.href='/ON'\">";
//  s += "<br><br><br>";
//  s += "<input type=\"button\" name=\"bi\" value=\"Turn LED OFF\" onclick=\"location.href='/OFF'\">";
//  s += "</html>\n";
//  //Serve the HTML document to the browser.
//  client.flush ();
//  //clear previous info in the stream
//  client.print (s); // Send the response to the client
//  delay(1);
//  Serial.println("Client disonnected" );
//  //Looking under the hood)
}
