#include <WiFi.h>
#include <WebServer.h>
#include <max6675.h>
#include <WiFiManager.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>

void displayTemperature(float temp);

// Define MAX6675 pins
int thermoDO = 19;
int thermoCS = 5;
int thermoCLK = 18;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN    26
#define DATA_PIN  25
#define CS_PIN    27

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

WebServer server(80);  // Server on port 80

void setup() {
    Serial.begin(115200);

    // WiFiManager, Local initialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    bool res;
    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
    } else {
        Serial.println("BBQ connected...yeey :)");
        P.begin();
        P.setIntensity(5); 
        P.displayClear();    
    }

    server.on("/", HTTP_GET, handleRoot);
    server.begin();
    delay(1000);  // Wait for MAX6675 to stabilize
}

void loop() {
    server.handleClient();  // Handle web server requests
  
    float tempF = celsiusToFahrenheit(thermocouple.readCelsius());
    displayTemperature(tempF);
    delay(2000);  // Update the display every second
}

float celsiusToFahrenheit(float celsius) {
    return celsius * 9.0/5.0 + 32.0;
}

void displayTemperature(float temp) {
    char tempStr[10];
    sprintf(tempStr, "%.0f F", temp);  // Convert float to string with 1 decimal place and append "F" for Fahrenheit
    P.displayText(tempStr, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    Serial.println(tempStr);
    P.displayAnimate();
}

void handleRoot() {
    String html = "<html><head>";
    html += "<meta http-equiv='refresh' content='30'>";
    html += "<style>";
    //  html += "body { display: flex; height: 100vh;  justify-content: center; font-family: Arial, sans-serif; }";
    html += "</style>";
    html += "</head><body>";
    html += "<div>";
    html += "<h1><center>My BBQ</center></h1>";
    float tempF = celsiusToFahrenheit(thermocouple.readCelsius());  // Reading temperature here just for the webpage display
    html += "<h1><center>Temperature = " + String(tempF) + " F</center></h1>";
    html += "<h1><center>at The Grey Fox</center></h1>";                                                            
    html += "</div>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}
