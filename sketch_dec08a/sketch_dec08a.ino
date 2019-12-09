
#include <ESP8266WiFi.h>
WiFiServer server(80); //Initialize the server on Port 80

int LED_PIN = 14;

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_AP); //Our ESP8266-12E is an AccessPoint
  WiFi.softAP("Hello_IoT", "12345678"); // Provide the (SSID, password); .
  server.begin(); // Start the HTTP Server

  //Looking under the hood
  Serial.begin(115200); //Start communication between the ESP8266-12E and the monitor window
  IPAddress HTTPS_ServerIP= WiFi.softAPIP(); // Obtain the IP of the Server
  Serial.print("Server IP is: "); // Print the IP to the monitor window
  Serial.println(HTTPS_ServerIP);

  pinMode(LED_PIN, OUTPUT); //GPIO14 is an OUTPUT pin;
  digitalWrite(LED_PIN, LOW); //Initial state is ON
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();
  if (!client) {
       return;
  }
  
  //Looking under the hood
  Serial.println("Somebody has connected :)");

  //Read what the browser has sent into a String class and print the request to the monitor
  String request =client.readStringUntil('\r');
  //Looking under the hood
  Serial.println(request);

  // Handle the Request

  if (request.indexOf("/ON") != -1){
    digitalWrite(LED_PIN, HIGH); }
  else if (request.indexOf("/OFF") != -1){
    digitalWrite(LED_PIN, LOW);
  }
  
  // Prepare the HTML document to respond and add buttons:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "<br><input type=\"button\" name=\"b1\" value=\"Turn LED ON\" onclick=\"location.href='/ON'\">";
  s += "<br><br><br>";
  s += "<input type=\"button\" name=\"bi\" value=\"Turn LED OFF\" onclick=\"location.href='/OFF'\">";
  s += "</html>\n";
  //Serve the HTML document to the browser.
  client.flush ();
  //clear previous info in the stream
  client.print (s); // Send the response to the client
  delay(1);
  Serial.println("Client disonnected" );
  //Looking under the hood)
  
}
