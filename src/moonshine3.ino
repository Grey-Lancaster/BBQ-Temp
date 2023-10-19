#include <WiFi.h>
#include <WebServer.h>
#include <max6675.h>
#include <WiFiManager.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>

void displayTemperature(float temp);

// Define MAX6675 pins
int thermoDO = 19;
int thermoCS = 18;
int thermoCLK = 5;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN    26
#define DATA_PIN  25
#define CS_PIN    27
//user defines/var
bool animate = 1;
unsigned long currentMillis = millis();
unsigned long previousMillis = millis();
int counter = 0;
int ip = 0;
#define BUF_SIZE  75
char curMessage[BUF_SIZE];

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

WebServer server(80);  // Server on port 80
char message[50];
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
        
        IPAddress theIP = WiFi.localIP();
        Serial.println(theIP);
        
 
        }
      currentMillis = millis();
    if (currentMillis - previousMillis > 10000)
    {
        previousMillis = currentMillis;
        animate = !animate;
        P.displayReset();
    }
    if (animate)
    {
     // Set up first message as the IP address

    sprintf(curMessage, "Point your broswer to Http://%03d.%03d.%03d.%03d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
    // P.displayText("Point your browser to " + String(myIP) , PA_CENTER, 110, 4500, PA_SCROLL_LEFT, PA_NO_EFFECT); 
    P.displayText(curMessage  , PA_CENTER, 110, 4500, PA_SCROLL_LEFT, PA_NO_EFFECT);   

    while (!P.displayAnimate());
    }   

    server.on("/", HTTP_GET, handleRoot);
    server.begin();
      // Wait for MAX6675 to stabilize
    delay(1000);

     
}

void loop() {

    server.handleClient();  // Handle web server requests
  
    float tempF = celsiusToFahrenheit(thermocouple.readCelsius());
    displayTemperature(tempF);
    delay(5000);  // Update the display every 5 seconds
    // The temp does not update without this delay
      }  

float celsiusToFahrenheit(float celsius) {
    return celsius * 9.0/5.0 + 32.0;
}

void displayTemperature(float temp) {
    char tempStr[10];
 //   sprintf(tempStr, "%.1f F", temp);  // Convert float to string with 1 decimal place and append "F" for Fahrenheit
    sprintf(tempStr, "%.0f F", temp);  // Convert float to string with 0 decimal place and append "F" for Fahrenheit
     P.displayText(tempStr, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT); // this display on the MAX7219
    float tempF = celsiusToFahrenheit(thermocouple.readCelsius());  // Reading temperature here just for the serial display 
    Serial.println(String(tempF));
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
