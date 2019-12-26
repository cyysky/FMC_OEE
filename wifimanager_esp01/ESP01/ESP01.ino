

#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal

#include <WiFiManager.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#include <EEPROM.h>




//Web server
ESP8266WebServer server2(888);


// GPIO Pin
int LED_PIN = 14;
int INPUT_PIN = 0;
int LED_PIN_ESP = 2;
int WIFI_PIN = 15;

// Input Status and debouncing
int inputValue = 0;
int inputState;
int lastInputState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

String strText ;

boolean ledState;
unsigned long ms;        //time from millis()
unsigned long msLast;    //last time the LED changed state


// HTML web page to handle 3 input fields (input1, input2, input3)
//const char index_html[] PROGMEM = R"=====(
//<!DOCTYPE HTML><html><head>
//  <title>ESP Input Form</title>
//  <meta name="viewport" content="width=device-width, initial-scale=1">
//  </head><body>
//  <form action="/get">
//    input1: <input  type="text" name="counter">
//    <input type="submit" value="Submit">
//  </form><br>
//</body></html>)=====";

String postForms1 = "<html>\
  <head>\
    <title>Machine Counter Link</title>\
  </head>\
  <body>\
    <h1>Machine Counter HTTP Link</h1><br>\
    <form method=\"post\" action=\"/submit/\">\
      <input style=\"width: 800px;\" type=\"text\" name=\'counter\' value=\'";


String postForms2 = "\'><br>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  </body>\
</html>";

//char str[512];
//WiFiManagerParameter custom_mqtt_server("server", "http server", str, 512);
//flag for saving data
bool shouldSaveConfig = false;
//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


void setup() {
  // Turn on Serial at 115200 bound speed
  Serial.begin(115200);
  Serial.println("\n\n Machine Industry of Tool ver 1.0.0.0 \n");
 /*                      *
          EEPROM Initialize
  *                       */

  EEPROM.begin(512);  //Initialize EEPROM
  //Read string from eeprom (testing eeprom)
  strText.reserve(512);
  strText = read_String(0);
  //  for(int i=0;i<512;i++)
  //  {
  //    strText = strText + char(EEPROM.read(i)); //Read one by one with starting address of 0x0F
  //  }
  
  Serial.println(strText.length());
  Serial.println("Saved string is : ");
  Serial.println(strText);  //Print the text
  Serial.println("\n");
  //strText.toCharArray(str, 100);
  //strcpy(str,strText);
  //Serial.println(str);
  //Serial.println("\n");
  
  const char * c = strText.c_str();
  Serial.println(c);
  /*											*
  				WIFI Manganger
  *												*/

  // WiFi Manager
  WiFiManager wifiManager;
  WiFiManagerParameter custom_mqtt_server("server", "http server", c,100);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  wifiManager.autoConnect();
  if (shouldSaveConfig==true){
      //read updated parameters
      strText = custom_mqtt_server.getValue();
      Serial.println("Writing " + strText);
      writeString(0, strText);  //Address 10 and String type data
  }
  // If you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  /*											*
  				GPIO Initialize
  *												*/

  // GPIO for Wemos D1 R2 SCK LED
  pinMode(LED_PIN, OUTPUT); //GPIO14 is an OUTPUT pin;
  digitalWrite(LED_PIN, LOW); //Initial state is ON

  //LED_PIN_ESP
  pinMode(LED_PIN_ESP, OUTPUT); //GPIO14 is an OUTPUT pin;
  digitalWrite(LED_PIN_ESP, HIGH); //Initial state is ON

  // INPUT GPIO
  pinMode(INPUT_PIN,INPUT); //GPIO 0 is an INPUT pin;
  //digitalWrite(INPUT_PIN, LOW); //Initial state is ON

  // WIFI MANAGER PIN
  pinMode(WIFI_PIN, INPUT_PULLUP); //GPIO 0 is an INPUT pin;
  //digitalWrite(WIFI_PIN, LOW); //Initial state is ON

 
  //  /*											*
  //  *				Web Server on port 8888
  //  *												*/
  //counterLink.reserve(500)
  server2.on("/", handleRoot); //Which routine to handle at root location
  server2.on("/submit/", handleSubmit);
  server2.begin();

}

void handleRoot() {
  server2.send(200, "text/html", postForms1 + strText + postForms2);
}

void handleSubmit() {
  Serial.println(server2.args());
  if (server2.args() > 0 ) {
    for ( uint8_t i = 0; i < server2.args(); i++ ) {
      //Serial.println(server2.argName(i));
      //Serial.println(server2.arg(i));

      //if((server2.arg(i)).indexOf("counter=")==0){
      //  Serial.println("found");
      //}

      if (server2.argName(i) == "counter") {
        Serial.println("Writing " + server2.arg(i));
        writeString(0, server2.arg(i));  //Address 10 and String type data
        strText = server2.arg(i);
        //do something here with value from server.arg(i);
      }

    }
  }
  server2.send(200, "text/html", postForms1 + strText + postForms2);
}

void writeString(char add, String data)
{ //https://circuits4you.com/2018/10/16/arduino-reading-and-writing-string-to-eeprom/

  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(add + i, data[i]);
  }
  EEPROM.write(add + _size, '\0'); //Add termination null character for String Data
  EEPROM.commit();
}

String read_String(char add)
{ //https://circuits4you.com/2018/10/16/arduino-reading-and-writing-string-to-eeprom/
  int i;
  char data[500]; //Max 500 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);
  //Serial.print(k);
  while (k != '\0' && len < 500) //Read until null character
  {
    k = EEPROM.read(add + len);
    //Serial.print(k);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}

void blinkLED(void)
{//https://forum.arduino.cc/index.php?topic=257095.0
    if (ms - msLast > (ledState ? 700 : 700)) {
        digitalWrite(LED_PIN_ESP, ledState = !ledState);
        msLast = ms;
    }
}

void loop() {
  ms = millis();
  blinkLED();
  
  if ( digitalRead(WIFI_PIN) == LOW ) {
    const char * c = strText.c_str();
    //WiFiManager
    WiFiManagerParameter custom_mqtt_server("server", "http server", c, 512);
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    wifiManager.addParameter(&custom_mqtt_server);

    //reset settings - for testing
    //wifiManager.resetSettings();

    //sets timeout until configuration portal gets turned off
    //useful to make it all retry or go to sleep
    //in seconds
    //wifiManager.setTimeout(120);

    //it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration

    //WITHOUT THIS THE AP DOES NOT SEEM TO WORK PROPERLY WITH SDK 1.5 , update to at least 1.5.1
    //WiFi.mode(WIFI_STA);

    //set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    if (!wifiManager.startConfigPortal("OnDemandAP")) {
    //if (!wifiManager.autoConnect("AutoConnectAP")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
    if (shouldSaveConfig==true){
      //read updated parameters
      strText = custom_mqtt_server.getValue();
      Serial.println("Writing " + strText);
      writeString(0, strText);  //Address 10 and String type data
    }
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }

  server2.handleClient();          //Handle client requests

  inputValue = digitalRead(INPUT_PIN);

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

      if (inputState == HIGH) {
        WiFiClient client;

        HTTPClient http;
        http.setTimeout(2000);
        Serial.print("[HTTP] begin...\n");
        if (http.begin(client, strText)) {  // HTTP


          Serial.print("[HTTP] GET...\n");
          // start connection and send HTTP header
          int httpCode = http.GET();

          // httpCode will be negative on error
          if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
              String payload = http.getString();
              Serial.println(payload);
            }
          } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
          }

          http.end();
        } else {
          Serial.printf("[HTTP} Unable to connect\n");
        }
      }

    }
  }

  lastInputState = inputValue;

}
