# SmrtWtch
A PoC smartwatch using ESP8266 
- This project is meant as a wearable companion to [Hercules](https://github.com/charromax/Hercules), my home automation system. Modules included in the project:
    = Wemos D1 for development/testing
    = ESP 12e for practical implementation
    = 128x64 OLED screen
    = TP 4056 battery charger module
    = MCP 1700 3v voltage regulator
    = 13300 Li/Ion battery

While still very much a work-in-progress, intended MVP features include:
  - Wifi Manager for implementation in different wifi networks without the need to save wifi info as part of the code
  - Current date / time in current timezone
  - Current weather info and weekly forecast
  - Hercules totem basic monitoring / control 
