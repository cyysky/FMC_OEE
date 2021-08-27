
/*#################################################################################################################

                                 SERIAL FUNCTIONS

  #################################################################################################################*/
/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    //Serial.println(inChar);
    // add it to the inputString:
    str_in += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      str_in_completed = true;
    }
  }
}

///////////////////////////////////////////////////// SOFTWARE SERIAL INPUT
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
//////////////////////////////////////////////////// TJC MSG TERMINATE BYTES
void tjc_send_end()
{
  TJC.write(0xff);
  TJC.write(0xff);
  TJC.write(0xff);
}
//////////////////////////////////////////////////// TJC MSG TERMINATE BYTES
void tjc_send_ip()
{
  sprintf(tjc_str, "ip.txt=\"%s\"", WiFi.localIP().toString().c_str()); // IP
  TJC.print(tjc_str);
  tjc_send_end();
  Serial.println(tjc_str);
}
