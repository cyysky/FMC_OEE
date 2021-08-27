
/*#################################################################################################################

                                   SETUP

  #################################################################################################################*/
void setup()
{
  ///////////////////////////////////////////////////////////////////////////// INIT COMMUNICATION
  Serial.begin(9600); // Initialize Serial to log output
  mySerial.begin(9600); // Initialize touchscreen serial
  Wire.begin(); //Joing I2C bus
  EEPROM.begin(512);  //Initialize EEPROM
  /////////////////////////////////////////////////////////////////////////////// PINS
  pinMode(LED_BUILTIN, OUTPUT); // D0 GPIO16 LED_BUILTIN LED pin as output
  ///////////////////////////////////////////////////////////////////////////// STRING VALUES
  // reserve 200 bytes for the inputString:
  str_in.reserve(200);
  ids.reserve(7);
  mqtt_topic.reserve(63);
  //ESP id
  snprintf (id, 7, "%06x", ESP.getChipId()) & 0xFFFFFF; //or uint64_t getEfuseMac()
  ids = String(id);
  Serial.println(ids);
  //###################### MQTT RESERVE
  mqtt_topic.reserve(63);
  mqtt_msg.reserve(150);
  ///////////////////////////////////////////////////////////////////////////// MLX90614
  if (therm.begin() == false) { // Initialize thermal IR sensor
    Serial.println("Qwiic IR thermometer did not acknowledge! Freezing!");
    while (1);
  }
  Serial.println("Qwiic IR Thermometer did acknowledge.");
  //float newEmissivity = 0.95;
  //therm.setEmissivity(newEmissivity);
  therm.setUnit(TEMP_C); // Set the library's units to Farenheit
  // Alternatively, TEMP_F can be replaced with TEMP_C for Celsius or
  // TEMP_K for Kelvin.
  ///////////////////////////////////////////////////////////////////////////// EEPROM
  init_wm();
  /////////////////////////////////////////////////////////////////////////////// MQTT
  psclient.setServer(sett.host, String(sett.port).toInt());
}
