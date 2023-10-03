
#define wifi_page_h

#include <Arduino.h>

const char indexPage[] PROGMEM = R"=====(<!doctype html>
<html>
<head>
  <title>ESP Wi-Fi Manager</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
  <div class="topnav">
    <h1>ESP Wi-Fi Manager</h1>
  </div>
  <div class="content">
    <div class="card-grid">
      <div class="card">
        <form action="/" method="POST">
          <p>
            <label for="ssid">SSID</label>
            <input type="text" id ="ssid" name="ssid"><br>
            <label for="pass">Password</label>
            <input type="text" id ="pass" name="pass"><br>
            <label for="ip">IP Address</label>
            <input type="text" id ="ip" name="ip" value="192.168.1.200"><br>
            <label for="gateway">Gateway Address</label>
            <input type="text" id ="gateway" name="gateway" value="192.168.1.1"><br>
            <input type ="submit" value ="Submit">
          </p>
        </form>
      </div>
    </div>
  </div>
</body>
</html>