#include <WiFi.h>
#include <WebServer.h>
#include <max6675.h>

// WiFi credentials
const char* ssid = "shop2";
const char* password = "mine0313";

// Define MAX6675 pins
int thermoDO = 19;
int thermoCS = 5;
int thermoCLK = 18;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

WebServer server(80);  // Server on port 80

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  connectToWiFi();

  // Configure WebServer routes
  server.on("/", HTTP_GET, handleRoot);
  
  server.begin();
  
  // Wait for MAX6675 to stabilize
  delay(500);
}

void loop() {
  server.handleClient();  // Handle web server requests
}

float celsiusToFahrenheit(float celsius) {
  return celsius * 9.0/5.0 + 32.0;
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to WiFi! IP Address: ");
  Serial.println(WiFi.localIP());
}

void handleRoot() {
  float tempF = celsiusToFahrenheit(thermocouple.readCelsius());
  
  String html = "<html><head>";
  html += "<meta http-equiv='refresh' content='10'>";
  html += "<style>";

//  html += "body { display: flex; height: 100vh;  justify-content: center; font-family: Arial, sans-serif; }";

  html += "</style>";
  html += "</head><body>";
  html += "<div>";
  html += "<h1><center>My BBQ</center></h1>";
  html += "<h1><center>Temperature = " + String(tempF) + " F</center></h1>";
  html += "<h1><center>at The Grey Fox</center></h1>";                                                            
  html += "</div>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}
