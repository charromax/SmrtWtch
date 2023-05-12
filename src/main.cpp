#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WIFI setup
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <resources.h>
#include <ArduinoJson.h>
#include "weather/OpenWeatherMap.h"

const char *ssid = "FUMANCHU";
const char *password = "heyholetsgo";
String nameOfCity = "Rosario,AR"; 
String apiKey = "bf71647e6f9abb1eac46b72ae665015e"; 

const long utcOffset = -10800;
WiFiClient client;
// Open Weather Map API server name
const char server[] = "api.openweathermap.org";
WiFiUDP servidorReloj;
NTPClient reloj(servidorReloj, "south-america.pool.ntp.org", utcOffset);
char weekDays[7][4] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"};
// Month names
String months[12] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sept", "Oct", "Nov", "Dic"};

#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0 // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define JSON_BUFF_DIMENSION 2500
unsigned long lastConnectionTime = 10 * 60 * 1000;     // last time you connected to the server, in milliseconds
const unsigned long postInterval = 10 * 60 * 1000;  // posting interval of 10 minutes  (10L * 1000L; 10 seconds delay for testing)
int jsonend = 0;
boolean startJson = false;
int status = WL_IDLE_STATUS;
String text;

void clearScreen()
{
  display.clearDisplay();
  display.display();
}

void drawCurrentDate()
{
  display.setTextColor(WHITE);
  display.setTextSize(1);
  reloj.update();
  String weekDay = weekDays[reloj.getDay()];
  time_t epochTime = reloj.getEpochTime();
  // Get a time structure
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;

  int currentMonth = ptm->tm_mon + 1;

  String currentMonthName = months[currentMonth - 1];

  // int currentYear = ptm->tm_year+1900;

  // Print complete date:
  String currentDate = String(currentMonthName) + ", " + String(monthDay);
  int posX = (SCREEN_WIDTH / 2) - (currentDate.length() * 3) + 1;
  display.setCursor(posX, 38);
  display.print(currentDate);
  display.display();
}

void drawBaseUi()
{
  display.clearDisplay();
  // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  display.drawBitmap(0, 0, baseUi, 64, 48, WHITE);
  drawCurrentDate();
  display.display();
  delay(500);
}

// WIFI SETUP

void setupWifi()
{
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
}


void drawCurrentTime()
{
  delay(500);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  reloj.update();
  String minutes = "";
  if (reloj.getMinutes() < 10)
  {
    minutes = "0" + (String)reloj.getMinutes();
  }
  else
  {
    minutes = (String)reloj.getMinutes();
  }
  String hora = (String)reloj.getHours() + ":" + minutes;
  int posX = (SCREEN_WIDTH / 2) - (hora.length() * 3);
  display.setCursor(posX, 3);
  display.print(hora);
  display.display();
}

void drawWeatherIcon()
{
  int posY = (SCREEN_HEIGHT / 2) - 8;
  display.drawBitmap(5, posY, chanceRain, 15, 15, WHITE);
  display.display();
}


//to parse json data recieved from OWM
void parseJson(const char * jsonString) {
  Serial.println(jsonString);
  
}

// to request data from OWM
void makehttpRequest() {
  // close any connection before send a new request to allow client make connection to server
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    // Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /data/2.5/forecast?q=" + nameOfCity + "&APPID=" + apiKey + "&mode=json&units=metric&cnt=2 HTTP/1.1");
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    
    char c = 0;
    while (client.available()) {
      c = client.read();
      // since json contains equal number of open and close curly brackets, this means we can determine when a json is completely received  by counting
      // the open and close occurences,
      //Serial.print(c);
      if (c == '{') {
        startJson = true;         // set startJson true to indicate json message has started
        jsonend++;
      }
      if (c == '}') {
        jsonend--;
      }
      if (startJson == true) {
        text += c;
      }
      // if jsonend = 0 then we have have received equal number of curly braces 
      if (jsonend == 0 && startJson == true) {
        parseJson(text.c_str());  // parse c string text in parseJson function
        text = "";                // clear text string for the next time
        startJson = false;        // set startJson to false to indicate that a new message has not yet started
      }
    }
  }
  else {
    // if no connction was made:
    Serial.println("connection failed");
    return;
  }
}

void checkWeather()
{
  //OWM requires 10mins between request intervals
  //check if 10mins has passed then conect again and pull
  if (millis() - lastConnectionTime > postInterval) {
    // note the time that the connection was made:
    lastConnectionTime = millis();

  }
}

void setup()
{
  Serial.begin(9600);
  setupWifi();
  // Iniciar pantalla OLED en la dirección 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  drawBaseUi();
}

void loop()
{
  drawBaseUi();
  drawCurrentTime();
  drawWeatherIcon();
  checkWeather();
  delay(1000);
}
