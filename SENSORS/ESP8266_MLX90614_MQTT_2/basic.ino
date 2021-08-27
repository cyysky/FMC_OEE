/*#################################################################################################################

                                 NON-BLOCK DELAY

  #################################################################################################################*/
bool is_ready(unsigned long *previous_time,unsigned long *timeout) {
  if ( millis() - *previous_time >= *timeout) {
    // save the last time you blinked the LED
    *previous_time =  millis();
    return true;
  } else {
    return false;
  }
}
void print_values()
{
  Serial.print(F("Temperature: "));
  Serial.print(therm.object());
  Serial.println(" C");
}

void do_read()
{
  digitalWrite(LED_BUILTIN, LOW);
  if (is_ready(&therm_previous_mills, &therm_delay)) {
    // Call therm.read() to read object and ambient temperatures from the sensor.
    if (therm.read()) // On success, read() will return 1, on fail 0.
    {
      // Use the object() and ambient() functions to grab the object and ambient
      // temperatures.
      // They'll be floats, calculated out to the unit you set with setUnit().
      //  Serial.print("Object: " + String(therm.object(), 2));
      //  Serial.println("C");
      //  Serial.print("Ambient: " + String(therm.ambient(), 2));
      //  Serial.println("F");
      //  Serial.println();
      /////////////////////////////////////////////////////////////////////////////// Update Screen Temperature
      TJC.print("t0.txt=\"" + String(therm.object(), 2) + "°C\"");
      tjc_send_end();
      ////////////////////////////////////////////////////////////////////////////// MQTT Publish Temperature
      mqtt_submit();
    }
  }
  digitalWrite(LED_BUILTIN, HIGH);
}
