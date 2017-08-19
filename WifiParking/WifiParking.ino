#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//#include <ESP8266HTTPClient.h>
//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include "ServiceConnector.h"

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN D3     // what digital pin we're connected to
// Define sensor module
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321

// select which pin will trigger the configuration portal when set to LOW
#define TRIGGER_PIN D1 //D8
#define TEST_PIN D2
#define MSG_FREQUENCY 4000

#define VERBOSE
#ifdef VERBOSE
#define VERBOSE_PRINT(str) Serial.println(str)
#else
#define VERBOSE_PRINT(str)
#endif

char ss_id[16]  ;
// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

ParkingService serverService;// = ParkingService( );

void setup() {
  // put your setup code here, to run once:
  serverService.configure("https://public.mobxy.com/sensor/sensor_parking_status_update.php","DE B8 19 67 F9 85 D0 D9 46 5A 95 4A D9 49 6D CF 80 53 CE 99","232" );
  Serial.begin(115200);
  VERBOSE_PRINT("\n Starting");
  pinMode(D4, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(TEST_PIN, INPUT_PULLUP);
    
  VERBOSE_PRINT("mi ssid: ");
  String mac = String(WiFi.macAddress()).substring(12);
  mac.replace(":","");
  String DeviceSSID = String(ss_id);
  DeviceSSID = "Parking_" + mac;
  DeviceSSID.toCharArray(ss_id,16);
  
  VERBOSE_PRINT(DeviceSSID);

  //Initialize sensor
  dht.begin();
}


void loop() {
  // is configuration portal requested?  
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;    
    VERBOSE_PRINT("Trigger pin");
    
    //reset settings - for testing
    //wifiManager.resetSettings();

    //sets timeout until configuration portal gets turned off
    //useful to make it all retry or go to sleep
    //in seconds
    //wifiManager.setTimeout(120);

    //it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration

    //WITHOUT THIS THE AP DOES NOT SEEM TO WORK PROPERLY WITH SDK 1.5 , update to at least 1.5.1
    //WiFi.mode(WIFI_STA);   
    if (!wifiManager.startConfigPortal(ss_id)) {
      VERBOSE_PRINT("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }
  // put your main code here, to run repeatedly:
  VERBOSE_PRINT("My IP is...");
  VERBOSE_PRINT(WiFi.localIP());
  digitalWrite(D4,HIGH );
  delay(1000);
  digitalWrite(D4,LOW );
  delay(1000);
  //Report Car Status
  reportCarStatus();

}

int getCurrentStatus()
{
  return digitalRead(TEST_PIN);
}

int lastStatus = -1;
int currentStatus = -1;
unsigned long lastSyncTime=0;
unsigned long now = 5000;

void reportCarStatus()
{
  //Refresh current values
  float temperature = dht.readTemperature(true);
  currentStatus = getCurrentStatus();
  now = millis();
  //Check if timeout
  //if( (now - lastStatus) > MSG_FREQUENCY && currentStatus != lastStatus ){
  if( currentStatus != lastStatus ){
    //Update last status
    lastStatus = currentStatus;
    lastSyncTime = now;
    VERBOSE_PRINT("millis");
    VERBOSE_PRINT(now);
    //Execute post
    serverService.post(currentStatus, temperature);
    //post();
  }  
}

