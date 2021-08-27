void do_cmd(){
  
  if (str_in_completed)
  {
    Serial.println(str_in);
    //################################################ TJC GET IP
    if (str_in.indexOf("ip") > -1)
    {
      tjc_send_ip();
    }
    //################################################ WIFIMANAGER CLOSE CONFIG PORTAL
    if (str_in.indexOf("0") > -1)
    {
      wm_stop_portal();
    }
    //################################################ GET SENSOR VALUES
    if (str_in.indexOf("values") > -1)
    {
      print_values(); // print sensors values
    }
    if (str_in.indexOf("setup") > -1)
    {
      wm_start_portal();
    }
    //################################################ CLEAN STRING
    str_in = ""; // clear the string:
    str_in_completed = false;
  }
}
