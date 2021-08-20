#include <ESP8266WiFi.h>     // ESP 8266
#include <Arduino.h>         //As Named
#include <dht.h>             //DTH11
#include <Adafruit_BMP280.h> //BMP280
#include <WiFiManager.h>     // WIFIManager
#include <SoftwareSerial.h>  // For touch screen TJC4024T032_011
#include <EEPROM.h>          // Storage For Strings
#include <PubSubClient.h>    // MQTT Client
/*#################################################################################################################

                                DEFINE

  #################################################################################################################*/
///////////////////////////////////////////////////// DHT
#define DHTTYPE DHT11  // DHT11 TYPE
#define DHT11_PIN D4   // DHT11 PIN
#define WIFIMGR_PIN D5 // WiFiManager's Setup Pin
///////////////////////////////////////////////////// LED
#define LED D0
///////////////////////////////////////////////////// TJC SOFTWARE SERIAL
#define TJC mySerial
#define TJC_RX D6
#define TJC_TX D7
/*#################################################################################################################

                                 OBJECTS

  #################################################################################################################*/
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
unsigned long wm_timeout = 600 * 1000; // milli seconds to run for
unsigned long wm_start_time = millis();
bool wm_portal_running = false;
//########################### WIFI manager setting
struct Settings
{
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
unsigned long mqtt_delay = 10000;
unsigned long mqtt_previous_mills = millis();
bool mqtt_connected = false;
/////////////////////////////////////////////////////////////////////////////   ESP CHIPID
char id[7];
String ids;
/////////////////////////////////////////////////////////////////////////////  NON-BLOCK DELAY
unsigned long sensors_delay = 1000;
unsigned long sensors_previous_mills = millis();
/////////////////////////////////////////////////////////////////////////////   Touch screen TJC4024T032_011
SoftwareSerial TJC(TJC_RX, TJC_TX); // RX, TX
char tjc_str[100];
///////////////////////////////////////////////////// DHT11
DHT dht = DHT(DHT11_PIN, DHTTYPE);
float dht_humidity = 0;
float dht_temperature = 0;
float dht_heat_index = 0;
///////////////////////////////////////////////////// BMP280
Adafruit_BMP280 bmp; // I2C
float bmp_temperature = 0;
float bmp_pressure = 0;
float bmp_altitude = 0;
/////////////////////////////////////////////////////////////////////////////   SERIAL input
String str_in = "";            // a String to hold incoming data
bool str_in_completed = false; // whether the string is complete
/*#################################################################################################################

                                 FUNCTION

  #################################################################################################################*/
//////////////////////////////////////////////////// NON-BLOCK DELAY
bool is_ready(unsigned long *previous_time, unsigned long *timeout)
{
  if (millis() - *previous_time >= *timeout)
  {
    // save the last time you blinked the LED
    *previous_time = millis();
    return true;
  }
  else
  {
    return false;
  }
}
//////////////////////////////////////////////////// WIFIMANAGER PARAMETER CALLBACK
void wm_save_callback()
{
  strncpy(sett.host, wmp_host.getValue(), 100);
  strncpy(sett.port, wmp_port.getValue(), 5);
  strncpy(sett.topic, wmp_topic.getValue(), 50);
  strncpy(sett.mac, wmp_mac.getValue(), 10);
  strncpy(sett.user, wmp_user.getValue(), 20);
  strncpy(sett.pass, wmp_pass.getValue(), 20);
  Serial.print("Host : ");
  Serial.print(sett.host);
  Serial.print(", Port : ");
  Serial.print(sett.port);
  Serial.print(", Topic : ");
  Serial.print(sett.topic);
  Serial.print("/");
  Serial.print(sett.mac);
  Serial.print(",User : ");
  Serial.print(sett.user);
  Serial.print(",Pass : ");
  Serial.println(sett.pass);
  EEPROM.put(0, sett);
  if (EEPROM.commit())
  {
    Serial.println("Settings saved");
  }
  else
  {
    Serial.println("EEPROM error");
  }
}
//////////////////////////////////////////////////// WIFIMANAGER LOOP ROUTINE
void do_wm()
{  
  wm.process();
  //////////////////////////////////////////////////// NON-BLOCK DELAY
  // is auto timeout portal running
  if (wm_portal_running)
  {
    //wm.process(); // do processing
    // check for timeout
    if (is_ready(&wm_start_time, &wm_timeout))
    {
      Serial.println("Portal timeout");
      wm_portal_running = false;
      wm.stopConfigPortal();
      wm.autoConnect();
    }
  }
}
//////////////////////////////////////////////////// READ SENSORS VALUES
void read_values()
{
  //////////////////////////////////////////////////// DHT11
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  dht_humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  dht_temperature = dht.readTemperature();
  // Compute heat index in Celsius (isFahreheit = false)
  dht_heat_index = dht.computeHeatIndex(dht_temperature, dht_humidity, false);
  ///////////////////////////////////////////////////// BMP280
  bmp_temperature = bmp.readTemperature();
  bmp_pressure = bmp.readPressure();
  bmp_altitude = bmp.readAltitude(1013.25); /* Adjusted to local forecast! */
}
///////////////////////////////////////////////////// PRINTS VALUES
void print_values()
{
  Serial.println("Hello World!");
  Serial.print(F("Humidity: "));
  Serial.print(dht_humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(dht_temperature);
  Serial.print(F("°C "));
  Serial.print(F(" Heat index: "));
  Serial.print(dht_heat_index);
  Serial.print(F("°C "));
  Serial.print(F("Temperature = "));
  Serial.print(bmp_temperature);
  Serial.println(" *C");
  Serial.print(F("Pressure = "));
  Serial.print(bmp_pressure);
  Serial.print(" Pa");
  Serial.print(F(" Approx altitude = "));
  Serial.print(bmp_altitude);
  Serial.println(" m");
}
//////////////////////////////////////////////////// TJC MSG TERMINATE BYTES
void tjc_send_end()
{
  TJC.write(0xff);
  TJC.write(0xff);
  TJC.write(0xff);
}
//////////////////////////////////////////////////// TJC
void update_screen_values()
{
  //用sprintf来格式化字符串，给n0的val属性赋值
  sprintf(tjc_str, "t0.txt=\"%sc\"", String(dht_temperature, 2).c_str()); // 温度 1
  //把字符串发送出去
  TJC.print(tjc_str);
  //发送结束符
  tjc_send_end();
  sprintf(tjc_str, "t1.txt=\"%sc\"", String(bmp_temperature, 2).c_str()); // 温度 2
  TJC.print(tjc_str);
  tjc_send_end();
  sprintf(tjc_str, "t2.txt=\"%s%%\"", String(dht_humidity, 1).c_str()); // 湿度
  TJC.print(tjc_str);
  tjc_send_end();
  sprintf(tjc_str, "t3.txt=\"%sc\"", String(dht_heat_index, 2).c_str()); // 热指数
  TJC.print(tjc_str);
  tjc_send_end();
  sprintf(tjc_str, "t4.txt=\"%sm\"", String(bmp_altitude, 2).c_str()); // 热指数
  TJC.print(tjc_str);
  tjc_send_end();
  sprintf(tjc_str, "t5.txt=\"%sp\"", String(bmp_pressure, 2).c_str()); // 热指数
  TJC.print(tjc_str);
  tjc_send_end();
}
///////////////////////////////////////////////////// MQTT SUBMIT
void mqtt_submit()
{
  if (is_ready(&mqtt_previous_mills, &mqtt_delay))
  {
    if (!wm_portal_running)
    {
      if (WiFi.status() == WL_CONNECTED)
      {

        //Serial.println("Wifi Connected");
        mqtt_connected = false;
        if (!psclient.connected())
        {
          if (psclient.connect(("ESP_" + ids).c_str(), sett.user, sett.pass))
          {
            Serial.print(("ESP_" + ids).c_str());
            Serial.println(" connected");
            mqtt_connected = true;
          }
          else
          {
            Serial.print("failed, rc=");
            Serial.println(psclient.state());
          }
        }
        else
        {
          mqtt_connected = true;
        }

        if (mqtt_connected)
        {
          mqtt_topic = "";
          mqtt_topic.concat(sett.topic);
          mqtt_topic.concat('/');
          mqtt_topic.concat(sett.mac);

          mqtt_msg = "";
          mqtt_msg.concat("{\"temperature_1\":");
          mqtt_msg.concat(String(String(dht_temperature)));
          mqtt_msg.concat(",\"temperature_2\":");
          mqtt_msg.concat(String(String(bmp_temperature)));
          mqtt_msg.concat(",\"humidity\":");
          mqtt_msg.concat(String(String(dht_humidity)));
          mqtt_msg.concat(",\"heat_index\":");
          mqtt_msg.concat(String(String(dht_heat_index)));
          mqtt_msg.concat(",\"altitude\":");
          mqtt_msg.concat(String(String(bmp_altitude)));
          mqtt_msg.concat(",\"pressure\":");
          mqtt_msg.concat(String(String(bmp_pressure)));
          mqtt_msg.concat("}");

          psclient.publish(mqtt_topic.c_str(), mqtt_msg.c_str());
          Serial.println(mqtt_msg.c_str());
        }
      }
    }
  }
}

//////////////////////////////////////////////////// SERIAL INPUT
void serialEvent()
{
  /*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
  */
  while (Serial.available())
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    str_in += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n')
    {
      str_in_completed = true;
    }
  }
}
///////////////////////////////////////////////////// TJC SERIAL INPUT
void software_serial_event()
{
  while (TJC.available())
  {
    // get the new byte:
    char inChar = (char)TJC.read();
    // add it to the inputString:
    if (!((inChar == (char)0x1A) || (inChar == (char)0xFF)))
    { // Clear FF FF FF 1A bytes feedback from TJC touch sreen
      str_in += inChar;
      //Serial.println(inChar,HEX);
    }
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n')
    {
      str_in_completed = true;
    }
  }
}
/*#################################################################################################################

                                 SETUP

  #################################################################################################################*/
void setup()
{
  //////////////////////////////////////////////////////////////////////////////// STRING VALUES
  //###################### SERIAL INPUT
  str_in.reserve(200); // reserve 200 bytes for the inputString:
  //###################### ESP ID
  ids.reserve(7);
  snprintf(id, 7, "%06x", ESP.getChipId()); // & 0xFFFFFF; //or uint64_t getEfuseMac()
  ids = String(id);
  Serial.println(ids);
  //###################### WIFIMANAGER SETTING SETT INIT
  sett.host[100] = '\0'; //Add null to end of string to avoid overflow
  sett.port[5] = '\0';
  sett.topic[50] = '\0';
  sett.mac[10] = '\0';
  sett.user[20] = '\0';
  sett.pass[20] = '\0';
  //###################### MQTT RESERVE
  mqtt_topic.reserve(63);
  mqtt_msg.reserve(150);
  ///////////////////////////////////////////////////////////////////////////// EEPROM
  EEPROM.begin(512); //Initialize EEPROM
  ///////////////////////////////////////////////////////////////////////////// GET WIFIMANAGER SETTING
  EEPROM.get(0, sett);
  Serial.print("Host : ");
  Serial.print(sett.host);
  Serial.print(", Port : ");
  Serial.print(sett.port);
  Serial.print(", Topic : ");
  Serial.print(sett.topic);
  Serial.print("/");
  Serial.print(sett.mac);
  Serial.print(",User : ");
  Serial.print(sett.user);
  Serial.print(",Pass : ");
  Serial.println(sett.pass);
  /////////////////////////////////////////////////////////////////////////////// PINOUT
  pinMode(LED, OUTPUT); // LED PIN
                        ////////////////////////////////////////////////////////////////////////////// SERIAL
  Serial.begin(9600);   // MAIN SERIAL
                        ////////////////////////////////////////////////////////////////////////////// TOUCH SCREEN SERIAL
  TJC.begin(9600);      //TOUCH SCREEN SERIAL
                        ////////////////////////////////////////////////////////////////////////////// DHT11
  dht.begin();          //INIT DHT AT DHT11_PIN
                        ////////////////////////////////////////////////////////////////////////////// BMP280
  //if (!bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
  if (!bmp.begin(0x76))
  { //https://forum.arduino.cc/t/bmp280-hw-611-e-p-280-library-sensor-fault/590528/2
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    while (1)
      delay(10);
  }
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  ///////////////////////////////////////////////////////////////////////////// WIFI MANAGER
  WiFi.mode(WIFI_STA);
  new (&wmp_host) WiFiManagerParameter("host", "mqtt domain", sett.host, 100);
  new (&wmp_port) WiFiManagerParameter("port", "mqtt port", sett.port, 5); //copy from AutoConnectNonBlockingwParams.ino
  new (&wmp_topic) WiFiManagerParameter("topic", "mqtt topic", sett.topic, 50);
  new (&wmp_mac) WiFiManagerParameter("mac", "machine identification", sett.mac, 10);
  new (&wmp_user) WiFiManagerParameter("user", "mqtt user", sett.user, 20);
  new (&wmp_pass) WiFiManagerParameter("pass", "mqtt password", sett.pass, 20);

  wm.addParameter(&wmp_host);
  wm.addParameter(&wmp_port);
  wm.addParameter(&wmp_topic);
  wm.addParameter(&wmp_mac);
  wm.addParameter(&wmp_user);
  wm.addParameter(&wmp_pass);
  wm.setConfigPortalBlocking(false); //for non blocking https://github.com/tzapu/WiFiManager/blob/master/examples/NonBlocking/AutoConnectNonBlockingwParams/AutoConnectNonBlockingwParams.ino
  wm.setSaveParamsCallback(wm_save_callback);

  std::vector<const char *> menu = {"wifi", "info", "param", "sep", "restart"}; //Advanced.ino
  wm.setMenu(menu);

  if (!wm.autoConnect())
  {
    Serial.println("Failed to connect or hit timeout");
    // ESP.restart();
  }
  else
  {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }
  /////////////////////////////////////////////////////////////////////////////// MQTT
  psclient.setServer(sett.host, String(sett.port).toInt());
}
/*#################################################################################################################

                                 LOOP

  #################################################################################################################*/
void loop()
{
  ///////////////////////////////////////////////////// WIFIMANGER PROCESS
  do_wm();
  ///////////////////////////////////////////////////// TJC SERIAL INPUT
  software_serial_event();
  /////////////////////////////////////////////////////////////////////////////// SENSOR UPDATE
  if (!wm_portal_running&&is_ready(&sensors_previous_mills, &sensors_delay))
  {
    digitalWrite(LED, HIGH); // LED LIGHT ON
    //################################################ READ DHT11 & BMP280 VALUES TO GLOBAL VARIABLE
    read_values();
    //################################################ UPDATE HMI SCREEN VALUES
    update_screen_values();
    //################################################ MQTT PUBLISH
    mqtt_submit();
    //################################################ LED
    digitalWrite(LED, LOW); // LED LIGHT OFF
  }
  /////////////////////////////////////////////////////////////////////////////// SERIAL INPUT
  if (str_in_completed)
  {
    Serial.println(str_in);
    //################################################ TJC GET IP
    if (str_in.indexOf("ip") > -1)
    {
      sprintf(tjc_str, "ip.txt=\"%s\"", WiFi.localIP().toString().c_str()); // IP
      TJC.print(tjc_str);
      tjc_send_end();
    }
    //################################################ WIFIMANAGER CLOSE CONFIG PORTAL
    if (str_in.indexOf("0") > -1)
    {
      if (wm_portal_running)
      {
        Serial.println("Page O Closing Portal");
        wm_portal_running = false;
        wm.stopConfigPortal();
        wm.autoConnect();
      }
    }
    //################################################ GET SENSOR VALUES
    if (str_in.indexOf("values") > -1)
    {
      print_values(); // print sensors values
    }
    if (str_in.indexOf("setup") > -1)
    {
      if (!wm_portal_running)
      {
        Serial.println("Setup, Starting Config Portal");
        wm.disconnect();
        wm.setConfigPortalBlocking(false);
        wm.startConfigPortal();
        wm_portal_running = true;
        wm_start_time = millis();
      }
    }
    //################################################ CLEAN STRING
    str_in = ""; // clear the string:
    str_in_completed = false;
  }
}