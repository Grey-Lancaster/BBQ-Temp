# CLAUDE.md

BBQ-Temp: ESP32 firmware that reads a MAX6675 thermocouple (BBQ pit temperature in degrees F), shows it on a 4-module MAX7219 LED matrix and serves it on a small web page. Public hobby project. `README.md` holds only a short description and a to-do list (text alerts via Twilio, charts, wiring diagram and a configurable page are still open items).

## Layout
- `platformio.ini` - single env `upesy_wroom` (ESP32, Arduino). Libraries: MD_MAX72XX 3.3.0, MD_Parola 3.6.1, MAX6675 (siruli), WiFiManager. The file also contains three stray `#include` lines under `framework`; they look accidental and PlatformIO ignores them, so tidy rather than copy them.
- `src/moonshine3.ino` - the whole app (about 190 lines): WiFiManager setup, scrolling IP message, temperature read and display every 10 s, `handleRoot()` web page that auto-refreshes every 30 s. PlatformIO builds it despite the `.ino` extension.
- `lib/wledfont/wledFont.h` - LED matrix font (from the wledPixel project, credited in the sketch); `lib/Wifipage.h` - an HTML page string.
- `data/` - `greyfox.jpg` and `favicon.jpg` for SPIFFS. They are only used if `USE_SPIFFS` is defined at the top of the sketch (commented out by default); upload them with `pio run -t uploadfs`.
- `index.html`, `manifest.json`, `firmware.bin` - browser installer (ESP Web Tools): the page flashes the committed `firmware.bin` at offset 0. Replacing that binary is a manual step.

## Build / flash
- `pio run` builds, `pio run -t upload` flashes (115200 baud). Pin assignments are in the sketch: thermocouple DO/CS/CLK = 19/18/5, LED matrix CLK/DATA/CS = 26/25/27; adjust them for different wiring.
- I found no tests and no CI. Verification is flashing a board with the probe and display attached.

## Gotchas
- The sketch starts WiFiManager's captive portal with a hard-coded AP name and password in `setup()`. Treat that as a placeholder; do not copy real credentials into the repo.
- If WiFi fails to connect the sketch only logs it and the display and web server are not set up properly (the "work without WiFi" to-do is still open).
- The degree symbol relies on the custom font and the UTF-8 source encoding.
- `firmware.bin` is a committed binary; do not rebuild and overwrite it unless asked.

## Release / deploy
- Push to `main`. There are no tags or releases, and no workflow. A `gh-pages` branch also exists and was merged into `main` earlier; I did not check how Pages is currently set up.
