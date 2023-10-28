// Included an option to post a small image on the website as well as a favicon.jpg

// I found the instructions for SPIFFS @ https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/

// I got wledfont.h from a great program @ https://github.com/widapro/wledPixel

// #define USE_SPIFFS  // Uncomment this line if you want to use SPIFFS.

#include <WiFi.h>
#include <WebServer.h>
#include <max6675.h>
#include <WiFiManager.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include "wledFont.h"
#include <FS.h>
#include <SPIFFS.h>


void displayTemperature(float temp);

// Define MAX6675 pins
int thermoDO = 19;
int thermoCS = 18;
int thermoCLK = 5;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4 //number of segments
#define CLK_PIN    26
#define DATA_PIN  25
#define CS_PIN    27

// HARDWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);


//user defines/var
bool animate = 1;
unsigned long currentMillis = millis();
unsigned long previousMillis = millis();
int counter = 0;
int ip = 0;
#define BUF_SIZE  75
char curMessage[BUF_SIZE];

WebServer server(80);  // Server on port 80
char message[50];
void setup() {
    Serial.begin(115200);
 if (!SPIFFS.begin()) {
    Serial.println("An error occurred while mounting SPIFFS");
 return;
}

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
    P.displayText(curMessage  , PA_CENTER, 50, 500, PA_SCROLL_LEFT, PA_NO_EFFECT);   

    while (!P.displayAnimate());
    }   

    server.on("/", HTTP_GET, handleRoot);

    #ifdef USE_SPIFFS
        server.on("/greyfox.jpg", HTTP_GET, []() {
        File file = SPIFFS.open("/greyfox.jpg", "r");
        server.streamFile(file, "image/jpg");
        file.close();
});  

    server.on("/favicon.jpg", HTTP_GET, []() {
        File file = SPIFFS.open("/favicon.jpg", "r");
        server.streamFile(file, "image/jpg");
        file.close();
});
#endif

    server.begin();

// Wait for MAX6675 to stabilize
//    delay(1000);
    P.setFont(0, wledFont_cyrillic);
     
}

unsigned long lastDisplayUpdate = 0; // Time when the display was last updated
unsigned long lastSerialPrint = 0;   // Time when the temperature was last printed to Serial
//const long displayInterval = 2000;   // 2 seconds
const long displayInterval = 10000;   // 20 seconds
const long printInterval = 10000;    // 20 seconds

void loop() {
    server.handleClient();

    unsigned long currentMillis = millis();

    // Update the display every 2 seconds
    if (currentMillis - lastDisplayUpdate >= displayInterval) {
        lastDisplayUpdate = currentMillis;
        float tempF = celsiusToFahrenheit(thermocouple.readCelsius());
        displayTemperature(tempF);
    }

    // Print to the Serial every 10 seconds
    if (currentMillis - lastSerialPrint >= printInterval) {
        lastSerialPrint = currentMillis;
        float tempF = celsiusToFahrenheit(thermocouple.readCelsius());
        Serial.println(String(tempF));
    }
        // Keep updating the display animation with each loop iteration
    P.displayAnimate();

}

float celsiusToFahrenheit(float celsius) {
    return celsius * 9.0/5.0 + 32.0;
}


char tempStr[20];


void displayTemperature(float temp) {
    sprintf(tempStr, "Pit  %.1f °F", temp); // Include "pit" before the temperature
    P.displayText(tempStr, PA_CENTER, 50, 10000, PA_SCROLL_LEFT, PA_SCROLL_LEFT); 
    P.displayAnimate();
    P.displayClear();

}


void handleRoot() {
    String html = "<html><head>";
    #ifdef USE_SPIFFS
    html += "<link rel=\"icon\" href=\"/favicon.jpg\" type=\"image/jpg\">"; // provides favicon.jpg
    #endif
    html += "<meta charset='UTF-8'>";  // Specify UTF-8 encoding
    html += "<meta http-equiv='refresh' content='30'>";
    html += "<style>";
    html += "body { display: flex; height: 100vh;  justify-content: center; font-family: Arial, sans-serif; }";
    html += "</style>";
    html += "</head><body>";
    html += "<div>";
    html += "<h1><center>My BBQ</center></h1>";
    float tempF = celsiusToFahrenheit(thermocouple.readCelsius());
    //html += "<h1><center>Temperature = " + String(tempF) + " F</center></h1>";  // This shows the Temp on the webpage with 2 decimiles
    html += "<h1><center>Temperature = " + String(tempStr) + "</center></h1>";  // This shows the Temp on the webpage with no decimles
    html += "<h1><center>at The Grey Fox</center></h1>";
    html += "</div>";
    #ifdef USE_SPIFFS
    html += "<img src='/greyfox.jpg' alt='Grey Fox'width='120' height='80'  >"; //This serves an image named greyfox.jpg from the SPIFFS  
    #endif    
    html += "</body></html>";
    server.send(200, "text/html", html);
}

