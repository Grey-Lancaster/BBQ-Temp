#include <WiFi.h>
#include <WebServer.h>
#include <max6675.h>
#include <WiFiManager.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
// WiFi credentials
//const char* ssid = "shop2";
//const char* password = "mine0313";

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

// Hardware SPI connection
//MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Arbitrary output pins
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);


WebServer server(80);  // Server on port 80

void setup() {
  Serial.begin(115200);

    // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want it.

    // put your setup code here, to run once:
    Serial.begin(115200);
   
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    // wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("BBQ connected...yeey :)");
    P.begin();
    P.setIntensity(5); 
    P.displayClear();    
  

}


  
  server.on("/", HTTP_GET, handleRoot);

  server.begin();

  // Wait for MAX6675 to stabilize
  delay(1000);




}


void loop() {
server.handleClient();  // Handle web server requests
}



float celsiusToFahrenheit(float celsius) {
  return celsius * 9.0/5.0 + 32.0;
}

 void displayTemperature(float temp) {
    char tempStr[10];
//    sprintf(tempStr, "%.1f F", temp);  // Convert float to string with 1 decimal place and append "F" for Fahrenheit
   sprintf(tempStr, "%.0f F", temp);  // Convert float to string with 1 decimal place and append "F" for Fahrenheit
    P.displayText(tempStr, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    Serial.println(tempStr);
    P.displayAnimate();
 }


void handleRoot() {


  
  float tempF = celsiusToFahrenheit(thermocouple.readCelsius());
  displayTemperature(tempF);
 P.displayAnimate();


  String html = "<html><head>";
  html += "<meta http-equiv='refresh' content='30'>";
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
