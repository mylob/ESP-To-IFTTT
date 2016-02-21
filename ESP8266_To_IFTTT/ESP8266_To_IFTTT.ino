#include <ESP8266WiFi.h>
#include <arduino.h>
#include "DataToMaker.h"

#define SERIAL_DEBUG // Uncomment this to dissable serial debugging

// define gpio pins here:
#define NUMBER_OF_SENSORS 2 // THIS MUST MATCH THE NUMBER OF SENSORS IN THE SENSOR ARRAY / NO MORE THAN 3

#define FRONT_DOOR_PIN 5 // GPIO5
#define GARAGE_DOOR_PIN 4 // GPIO4
// pin for heatbeat LED
#define HEARTBEAT_PIN 13 //GPIO13


// Define program constants

const char* myKey = "your key"; // your maker key here
const char* ssid = "your ssid"; // your router ssid here
const char* password = "your password"; // your router password here

// define program values
int sensors[NUMBER_OF_SENSORS] = {FRONT_DOOR_PIN, GARAGE_DOOR_PIN}; // place your defined sensors in the curly braces
String doorStates[2] = {"Closed", "Open"}; // You can change the LOW / HIGH state strings here



// declare new maker event with the name "ESP"
DataToMaker event(myKey, "ESP");

// LEAVE SET

int pvsValues[NUMBER_OF_SENSORS];
bool connectedToWiFI = false;

void setup()
{
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
  delay(200);
  Serial.println();
#endif

  delay(10); // short delay
  pinMode(HEARTBEAT_PIN, OUTPUT);
  for (int i = 0 ; i < NUMBER_OF_SENSORS ; i++)
  {
    pinMode(sensors[i], INPUT);
    pvsValues[i] = -1; // initialise previous values to -1 to force initial output
  }
  WiFi.mode(WIFI_STA);
  ConnectWifi();
}

void loop() {
  if (wifiConnected)
  {
    if (DetectChange())
    {
      debugln("connecting...");
      if (event.connect())
      {
        debugln("Connected To Maker");
        event.post();
      }
      else debugln("Failed To Connect To Wifi!");
    }
    delay(1000); // pause for 1 second
    digitalWrite(HEARTBEAT_PIN, !digitalRead(HEARTBEAT_PIN));
  }
  else
  {
    delay(60 * 1000); // 1 minute delay before trying to re connect
    ConnectWifi();
  }
}

bool ConnectWifi()
{
  // Connect to WiFi network
  debugln();
  debugln();
  debug("Connecting to ");
  debugln(ssid);
  unsigned long startTime = millis();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && startTime + 30 * 1000 >= millis()) {
    delay(500);
    debug(".");
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    debugln("");
    debugln("WiFi connected");
  }
  else
  {
    WiFi.disconnect();
    debugln("");
    debugln("WiFi Timed Out!");
  }
}

bool wifiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}


bool DetectChange()
{
  int val;
  bool changed = false;
  for (int i = 0 ; i < NUMBER_OF_SENSORS ; i++)
  {
    if ((val = digitalRead(sensors[i])) != pvsValues[i])
    {
      pvsValues[i] = val;
      event.setValue(i + 1, doorStates[val]);
      debug("Changes Detected On Value");
      debugln(String(i + 1));
      changed = true;
    }
  }
  if (!changed) debugln("No Changes Detected");
  return changed;
}


void debug(String message)
{
#ifdef SERIAL_DEBUG
  Serial.print(message);
#endif
}

void debugln(String message)
{
#ifdef SERIAL_DEBUG
  Serial.println(message);
#endif
}

void debugln()
{
#ifdef SERIAL_DEBUG
  Serial.println();
#endif
}


