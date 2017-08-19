

/*
  Service Registration.
*/
#ifndef ParkingService_h
#define ParkingService_h

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266HTTPClient.h>

class ParkingService
{
  public:
    //ParkingService( );
    void configure(char *serviceUrl, char *fingerprint, char *deviceId);
    //void post3(int status);
    void post(int status, float temperature);    
  private:
    char *_serviceUrl;
    char *_deviceId;
    char *_fingerprint;
    //WiFiClient *_client;
    //HttpClient client;
    HTTPClient client;
};

#endif
