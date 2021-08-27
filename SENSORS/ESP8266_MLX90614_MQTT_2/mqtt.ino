/*#################################################################################################################

                                   MQTT

  #################################################################################################################*/
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
          mqtt_msg.concat("{\"temperature\":");
          mqtt_msg.concat(String(therm.object()));
          mqtt_msg.concat("}");

          psclient.publish(mqtt_topic.c_str(), mqtt_msg.c_str());
          Serial.println(mqtt_msg.c_str());
        }
      }
    }
  }
}
