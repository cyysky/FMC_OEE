/******************************************************************************
 * 
 *  Thermal with 3.2touch screen  * 
 *  
 *****************************************************************************/

/******************************************************************************
  Jim Lindblom @ SparkFun Electronics
  October 23, 2015
  https://github.com/sparkfun/SparkFun_MLX90614_Arduino_Library
  SparkFun IR Thermometer Evaluation Board - MLX90614
******************************************************************************/
/*#################################################################################################################
                                    HEADER
  #################################################################################################################*/
#include <ESP8266WiFi.h> // ESP 8266
#include <WiFiManager.h> // WIFIManager
#include <Wire.h> // I2C library, required for MLX90614
#include <SparkFunMLX90614.h> //Click here to get the library: http://librarymanager/All#Qwiic_IR_Thermometer by SparkFun
#include <SoftwareSerial.h> // For touch screen TJC3224T022_011
#include <EEPROM.h> // Storage For Strings 
#include <PubSubClient.h>
///////////////////////////////////////////////////// TJC SOFTWARE SERIAL
#define TJC mySerial
#define TJC_RX D6
#define TJC_TX D7
/*#################################################################################################################
                                   Global object
  #################################################################################################################*/
/////////////////////////////////////////////////////////////////////////////   Touch screen TJC3224T022_011
SoftwareSerial mySerial(TJC_RX, TJC_TX); 
char tjc_str[100];
/////////////////////////////////////////////////////////////////////////////   MLX90614
IRTherm therm; // Create an IRTherm object to interact with throughout
unsigned long therm_delay = 100; //max 65535
unsigned long therm_previous_mills = 0;
/////////////////////////////////////////////////////////////////////////////   ESP CHIPID
char id[7];
String ids;
/////////////////////////////////////////////////////////////////////////////   SERIAL input
String str_in = "";         // a String to hold incoming data
bool str_in_completed = false;  // whether the string is complete
/////////////////////////////////////////////////////////////////////////////   WIFI manager
//copy from https://github.com/tzapu/WiFiManager/blob/master/examples/Advanced/Advanced.ino
WiFiManager wm;
WiFiManagerParameter wmp_host;
WiFiManagerParameter wmp_port;
WiFiManagerParameter wmp_topic;
WiFiManagerParameter wmp_mac;
WiFiManagerParameter wmp_user;
WiFiManagerParameter wmp_pass;
//########################### WIFI manager setting
unsigned long  wm_timeout   = 120 * 1000; // seconds to run for
unsigned long wm_start_time = millis();
bool wm_portal_running      = false;
////////////////////// WIFI manager setting
struct Settings {
  char host[101]; // last as '\0' to avoid overflow
  char port[6];
  char topic[51];
  char mac[11];
  char user[21];
  char pass[21];
} sett;
/////////////////////////////////////////////////////////////////////////////   MQTT
WiFiClient wclient; //https://gist.github.com/vlna/b8268e6a143f2db10e84c59b5074f5b7
PubSubClient psclient(wclient);
String mqtt_topic; //string length 63 (topic 51 + '/' + mac 11)
String mqtt_msg;   //string length 150 for json
unsigned long mqtt_delay = 10 * 1000;
unsigned long mqtt_previous_mills = millis();
bool mqtt_connected = false;
