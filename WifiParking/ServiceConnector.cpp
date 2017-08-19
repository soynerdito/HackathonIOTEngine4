#include <ArduinoJson.h>
#include "ServiceConnector.h"


/*ParkingService::ParkingService(){  
  //_client = wifiClient;  
}*/

void ParkingService::configure(char *serviceUrl,char *fingerprint, char *deviceId){
  _serviceUrl = serviceUrl;
  _fingerprint = fingerprint;
  _deviceId = deviceId; 
}

//StaticJsonBuffer<50> JSONbuffer;   //Declaring static JSON buffer
void ParkingService::post(int status, float temperature)
{ 

  StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer  
  JsonObject& JSONencoder = JSONbuffer.createObject();  
  JSONencoder["id"] = _deviceId;
  JSONencoder["status"] = status;
  JSONencoder["temperature"] = temperature;
  
    
  client.begin(_serviceUrl, _fingerprint);      //Specify request destination
  client.addHeader("Content-Type", "application/json");  //Specify content-type header

  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("JsonMessageBuffer:");
  Serial.println(JSONmessageBuffer);

  int httpCode = client.POST(JSONmessageBuffer);   //Send the request
  String payload = client.getString();                                        //Get the response payload
  Serial.println("Payload:");
  Serial.println(payload);    //Print request response payload
  Serial.println("Http Code:");
  Serial.println(httpCode);   //Print HTTP return code
  
  client.end();  //Close connection
}


