// Copied from BasicHTTPSClient.ino created on: 20.08.2018

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h> // or the name could be in CAPS Arduino_JSON.h

// The Wifi details
const char* SSID = "Someplace Else 2.4G";
const char* pwd = "46123862";

String PARAM_SENSOR_NAME = "\"SensorName__c\"";
String Bearer = "Bearer";

// Define oAuth2 paramaters
String grant_type = "password";
String username = "aritram1@gmail.com.lwc";
String password = "Aritra1985###";
String client_id = "3MVG9n_HvETGhr3DyoS1cwXsp_QLSbEuBw91e.8OZ0i6y0DyMjtzTSMeAUuYnuDPEOCdFiAdRiXfWGj142YcO";
String client_secret = "EC6519F8A1A239EBD4DB506643A7AD494C8BB7AC4206E9AA0784B0D76C9C3F2E";
const uint8_t fingerprintForToken[20] = { 0xAE, 0x3D, 0xDC, 0x91, 0xBA, 0x5C, 0x54, 0x6D, 0x79, 0x1C, 0x7F, 0x72, 0xB8, 0xB4, 0xC8, 0x91, 0x4B, 0xC8, 0x29, 0x61 };

const uint8_t fingerprintForPostData[20] = { 0xEE, 0xF9, 0x79, 0x46, 0x67, 0x40, 0x87, 0x6D, 0x32, 0xB3, 0x81, 0x6E, 0xFC, 0xC9, 0x7E, 0xA6, 0xF0, 0x39, 0x4A, 0x04};

String TOKEN_ENDPOINT = "https://login.salesforce.com/services/oauth2/token";
String IOT_PE_ENDPOINT = "https://lwc-aritra-dev-ed.my.salesforce.com/services/data/v56.0/sobjects/IOTEvent__e";

String access_token = "";
String instance_url = "";
String id = "";
String token_type = "";
String issued_at = "";
String signature = "";

// Create Wifi, Cleint and Https conenction for communication
ESP8266WiFiMulti WiFiMulti;

DynamicJsonDocument jsonResponse(1024);

void setup() {

  Serial.begin(9600);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.localIP());
  WiFiMulti.addAP(SSID, pwd);
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    Serial.println("Local IP");
    Serial.println(WiFi.localIP());

    if(access_token == ""){ // || issued_at < now()){
      delay(2000);
      Serial.println("when i am blank");
      getToken();
    }
    else{
      delay(2000);
      Serial.println("when not blank!");
      Serial.print(access_token);
      Serial.println("new access token");
      
      postDataToSF();
    }
  }

  Serial.println("Wait 5s before next round...");
  delay(5000);
}

// method for encoding strings
String urlencode(String str){
  String encodedString="";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i =0; i < str.length(); i++){
    c=str.charAt(i);
    if (c == ' '){
      encodedString+= '+';
    } else if (isalnum(c)){
      encodedString+=c;
    } else{
      code1=(c & 0xf)+'0';
      if ((c & 0xf) >9){
          code1=(c & 0xf) - 10 + 'A';
      }
      c=(c>>4)&0xf;
      code0=c+'0';
      if (c > 9){
          code0=c - 10 + 'A';
      }
      code2='\0';
      encodedString+='%';
      encodedString+=code0;
      encodedString+=code1;
    }
    yield();
  }
  return encodedString;
}

void getToken(){
    // Define client and assign the finger print for https communication
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerprintForToken); // for insecure connection use : client->setInsecure();
    
    HTTPClient httpsTokenClient;

    if (httpsTokenClient.begin(*client, TOKEN_ENDPOINT)) {

      // Generate https body and header
      String body = "";
      body += "grant_type=" + urlencode(grant_type) + "&";
      body += "username=" + urlencode(username) + "&";
      body += "password=" + urlencode(password) + "&";
      body += "client_id=" + urlencode(client_id) + "&";
      body += "client_secret=" + urlencode(client_secret);
      Serial.println(body);

      httpsTokenClient.addHeader("content-type", "application/x-www-form-urlencoded");
      int httpCode = httpsTokenClient.POST(body);
      
      if (httpCode == 200 || httpCode == 201){
        Serial.printf("In Status: %d\n", httpCode);
        String textResponse = httpsTokenClient.getString();
        deserializeJson(jsonResponse, textResponse);
        
        const char* atoken = jsonResponse["access_token"];
        access_token = atoken; // So we need to get the output first in a pointer and then transfer its value to String - whih is weird and so the following direct assignment does not work!! LOL! 
                               // access_token = jsonResponse["access_token"];
        
        const char* is_at = jsonResponse["issued_at"];
        issued_at = is_at;
        
        const char* inst_url = jsonResponse["instance_url"];
        instance_url = inst_url;

        Serial.printf("from getToken access_token : %s \n", access_token);
        Serial.printf("from getToken issued_at : %s \n", issued_at);
        Serial.printf("from getToken instance_url : %s \n", instance_url);
      } 
      // httpCode will be negative on error
      else {
        Serial.printf("Not in Status 200 : %s\n", httpsTokenClient.errorToString(httpCode).c_str());
        String textResponse1 = httpsTokenClient.getString();
        deserializeJson(jsonResponse, textResponse1);
      }

      // close the https connection;
      httpsTokenClient.end();

    } 
    else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
}

// This method posts data to Salesforce
void postDataToSF(){

  std::unique_ptr<BearSSL::WiFiClientSecure>client2(new BearSSL::WiFiClientSecure);
  client2->setFingerprint(fingerprintForPostData); // for insecure connection use : client->setInsecure();
  HTTPClient httpsPostDataClient;

  //Serial.printf("from postDataToSF new at : %s \n", at);

  Serial.printf("from postDataToSF access_token : %s \n", access_token);
  Serial.printf("from postDataToSF issued_at : %s \n", issued_at);
  Serial.printf("from postDataToSF instance_url : %s \n", instance_url);
  
  if(httpsPostDataClient.begin(*client2, IOT_PE_ENDPOINT)){
    httpsPostDataClient.addHeader("content-type", "application/json");
    httpsPostDataClient.addHeader("authorization", Bearer + " " + access_token);

    String body = "{";
    body += PARAM_SENSOR_NAME + ":" + "\"Some random sensor\"";
    // body += "username=" + urlencode(username) + "&";
    // body += "password=" + urlencode(password) + "&";
    // body += "client_id=" + urlencode(client_id) + "&";
    // body += "client_secret=" + urlencode(client_secret);
    body += "}";
    body = "{\"SensorName__c\":\"Some random sensor3\"}";
    
    Serial.println(body);
    int httpCode = httpsPostDataClient.POST(body);
    if (httpCode == 200 || httpCode == 201) {
      String textResponse = httpsPostDataClient.getString();
      Serial.print(textResponse);
      deserializeJson(jsonResponse, textResponse);
    } 
    else {
      Serial.printf("Status is : %d : %s\n", httpCode, httpsPostDataClient.errorToString(httpCode).c_str());
      String textResponse1 = httpsPostDataClient.getString();
      deserializeJson(jsonResponse, textResponse1);
    }
    httpsPostDataClient.end(); // close the https connection;
  }
  else{
      Serial.printf("[HTTPS] Unable to connect to : %s", TOKEN_ENDPOINT);
  }

}
