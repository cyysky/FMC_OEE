/*#################################################################################################################

                                  WIFIMANAGER FUNCTIONS

  #################################################################################################################*/
//////////////////////////////////////////////////// INITIALISE WIFIMANAGER
void init_wm() {
  //EEPROM.begin(512);  //Initialize EEPROM done at setup
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
  //Add null to end of string to avoid overflow
  sett.host[100] = '\0';
  sett.port[5] = '\0';
  sett.topic[50] = '\0';
  sett.mac[10] = '\0';
  sett.user[20] = '\0';
  sett.pass[20] = '\0';

  ///////////////////////////////////////////////////////////////////////////// WIFI MANAGER
  new (&wmp_host) WiFiManagerParameter("host", "mqtt domain", sett.host, 100);
  new (&wmp_port) WiFiManagerParameter("port", "mqtt port", sett.port, 5); //copy from AutoConnectNonBlockingwParams.ino
  new (&wmp_topic) WiFiManagerParameter("topic", "mqtt topic",  sett.topic, 50);
  new (&wmp_mac) WiFiManagerParameter("mac", "machine identification", sett.mac, 10);
  new (&wmp_user) WiFiManagerParameter("user", "mqtt user",  sett.user, 20);
  new (&wmp_pass) WiFiManagerParameter("pass", "mqtt password", sett.pass, 20);

  wm.addParameter(&wmp_host);
  wm.addParameter(&wmp_port);
  wm.addParameter(&wmp_topic);
  wm.addParameter(&wmp_mac);
  wm.addParameter(&wmp_user);
  wm.addParameter(&wmp_pass);
  
  wm.setConfigPortalBlocking(false); //for non blocking https://github.com/tzapu/WiFiManager/blob/master/examples/NonBlocking/AutoConnectNonBlockingwParams/AutoConnectNonBlockingwParams.ino
  wm.setSaveParamsCallback(wm_save_callback);

  std::vector<const char *> menu = {"wifi", "info", "param", "sep", "restart", "exit"};
  wm.setMenu(menu);
  wm.setConnectTimeout(5);
  wm.autoConnect();

}
////////////////////////////////////////////////// START WIFIMANAGER PORTAL
void wm_start_portal()
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
////////////////////////////////////////////////// STOP WIFIMANAGER PORTAL
void wm_stop_portal()
{
  if (wm_portal_running)
  {
    Serial.println("Closing Portal");
    wm_portal_running = false;
    wm.stopConfigPortal();
    wm.autoConnect();
  }
}
//////////////////////////////////////////////////// WIFIMANAGER PARAMETER CALLBACK
void wm_save_callback() {
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
  if (EEPROM.commit()) {
    Serial.println("Settings saved");
  } else {
    Serial.println("EEPROM error");
  }
}
//////////////////////////////////////////////////// WIFIMANAGER LOOP ROUTINE
void do_wm() {
  wm.process();
  // is auto timeout portal running
  if (wm_portal_running && is_ready(&wm_start_time, &wm_timeout)) {
    // check for timeout
    Serial.println("Portal timeout");
    wm_stop_portal();
  }
}
