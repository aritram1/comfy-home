#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

// The Wifi details
const char* SSID = "OvaltineTea10";
const char* pwd = "ovaltineiphone";

// Mapped SF Fields to hold data
String PARAM_SENSOR_NAME = "SensorName__c";
String PARAM_PAYLOAD_NAME = "Payload__c";

String PARAM_SENSOR_NAME_A0 = "A0";
String PARAM_SENSOR_NAME_D2 = "D2";
String PARAM_SENSOR_NAME_D3 = "D3";
uint8_t PARAM_SENSOR_VALUE_A0 = 0;
uint8_t PARAM_SENSOR_VALUE_D2 = 0;
uint8_t PARAM_SENSOR_VALUE_D3 = 0;
    

// Define oAuth2 paramaters
String grant_type = "password";
String username = "aritram1@gmail.com.lwc";
String password = "*****"; //masked
String client_id = "***"; //masked
String client_secret = "***"; //masked
String TOKEN_ENDPOINT = "https://login.salesforce.com/services/oauth2/token";
String IOT_PLATFORMEVENT_ENDPOINT = "https://lwc-aritra-dev-ed.my.salesforce.com/services/data/v56.0/sobjects/IOTEvent__e";
const uint8_t fingerprintForToken[20] = { 0xAE, 0x3D, 0xDC, 0x91, 0xBA, 0x5C, 0x54, 0x6D, 0x79, 0x1C, 0x7F, 0x72, 0xB8, 0xB4, 0xC8, 0x91, 0x4B, 0xC8, 0x29, 0x61 };
const uint8_t fingerprintForPostData[20] = { 0xEE, 0xF9, 0x79, 0x46, 0x67, 0x40, 0x87, 0x6D, 0x32, 0xB3, 0x81, 0x6E, 0xFC, 0xC9, 0x7E, 0xA6, 0xF0, 0x39, 0x4A, 0x04};

// OAuth2 params are saved here
String access_token = "";
String instance_url = "";
String id = "";
String token_type = "";
String issued_at = "";
String signature = "";
String Bearer = "Bearer";

// Create Wifi, Cleint and Https conenction for communication
ESP8266WiFiMulti WiFiMulti;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  Serial.println("Local IP");
  Serial.println(WiFi.localIP());
  WiFiMulti.addAP(SSID, pwd);
}

void loop() {

  // measure the sensor data
  measureSensors();
    
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    Serial.println("Local IP");
    Serial.println(WiFi.localIP());

    if(access_token == ""){ // || issued_at < now()){
      Serial.println("Token Call to Salesforce Start");
      digitalWrite(LED_BUILTIN, HIGH);
      getToken();
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Token Call to Salesforce End");
    }
    else{
      Serial.println("Post Call to Salesforce Start");
      digitalWrite(LED_BUILTIN, HIGH);
      postDataToSF();
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("Post Call to Salesforce End");
    }
    Serial.println("Wait 5s before next round...");
    delay(5000);
  }
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
        
        // Parse JSON payload
        const size_t capacity = 512; //JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
        DynamicJsonDocument jsonDoc(capacity);
        DeserializationError error = deserializeJson(jsonDoc, textResponse);
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
        }

        // Decode JSON/Extract values
        Serial.println(F("Response:"));
        const char* _access_token = jsonDoc["access_token"];
        const char* _issued_at = jsonDoc["issued_at"];
        const char* _instance_url = jsonDoc["instance_url"];
        const char* _signature = jsonDoc["signature"];
        const char* _id = jsonDoc["id"];
        access_token = _access_token;
        issued_at = _issued_at;
        instance_url = _instance_url;
        id = _id;
        signature = _signature;
        
        Serial.println();
        Serial.println("access token");
        Serial.println(access_token);
        /*Serial.printf("from getToken issued_at : %s \n", issued_at);
        Serial.printf("from getToken instance_url : %s \n", instance_url);
        Serial.printf("from getToken id : %s \n", id);
        Serial.printf("from getToken signature : %s \n", signature);*/

      } 
      // else means this is not a successful response
      else {
        String textResponseError = httpsTokenClient.getString();
        Serial.printf("Not in Status 200 : %s\n", httpsTokenClient.errorToString(httpCode).c_str());
        Serial.println(textResponseError);
      }

      // close the https connection;
      httpsTokenClient.end();

    } 
    else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
}

// Method to measure the sensor values and store in global variables for further use
void measureSensors(){
  PARAM_SENSOR_VALUE_A0 = analogRead(A0);
  PARAM_SENSOR_VALUE_D2 = digitalRead(D2);
  PARAM_SENSOR_VALUE_D3 = digitalRead(D3);
  Serial.println("======measureSensors=====");
  Serial.println(PARAM_SENSOR_VALUE_A0);
  Serial.println(PARAM_SENSOR_VALUE_D2);
  Serial.println(PARAM_SENSOR_VALUE_D3);
  Serial.println("======measureSensors=====");
}

// This method posts data to Salesforce
void postDataToSF(){

  std::unique_ptr<BearSSL::WiFiClientSecure>client2(new BearSSL::WiFiClientSecure);
  client2->setFingerprint(fingerprintForPostData); // for insecure connection use : client->setInsecure();
  HTTPClient httpsPostDataClient;

  // Print to check the value is retained between calls
  // Serial.printf("from postDataToSF access_token : %s \n", access_token);
  // Serial.printf("from postDataToSF issued_at : %s \n", issued_at);
  // Serial.printf("from postDataToSF instance_url : %s \n", instance_url);
  
  if(httpsPostDataClient.begin(*client2, IOT_PLATFORMEVENT_ENDPOINT)){
    httpsPostDataClient.addHeader("content-type", "application/json");
    httpsPostDataClient.addHeader("authorization", Bearer + " " + access_token);

    /*
    // To be checked further how to send multiple parameters in JSON
    ///////////////////////////////////////////////////////////////
    String bodyString;
    StaticJsonDocument<768> bodyJson;
    bodyJson[PARAM_SENSOR_NAME_A0] = (String)PARAM_SENSOR_VALUE_A0;
    bodyJson[PARAM_SENSOR_NAME_D2] = (String)PARAM_SENSOR_VALUE_D2;
    bodyJson[PARAM_SENSOR_NAME_D3] = (String)PARAM_SENSOR_VALUE_D3;    
    serializeJson(bodyJson, bodyString);

    Serial.println("===bodystring=====");
    Serial.println(bodyString);
    Serial.println("========");
    ////////////////////////////////////////////////////////////////
    */
    
    String data = "";
    data += PARAM_SENSOR_NAME_A0 + "-" + PARAM_SENSOR_VALUE_A0 + "__";
    data += PARAM_SENSOR_NAME_D2 + "-" + PARAM_SENSOR_VALUE_D2 + "__";
    data += PARAM_SENSOR_NAME_D3 + "-" + PARAM_SENSOR_VALUE_D3;
    String body = "{";
    body += "\"" + PARAM_PAYLOAD_NAME + "\"" + ":" + "\"" + data + "\"";
    body += "}";

    Serial.println("====body====");
    Serial.println(body);
    Serial.println("========");
    
    const size_t capacity = 768; //JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
    DynamicJsonDocument jsonPostDoc(capacity);
        
    int httpCode = httpsPostDataClient.POST(body);
    if (httpCode == 200 || httpCode == 201) {
      String textResponsePost = httpsPostDataClient.getString();
      Serial.println("Response of /POST");
      Serial.println(textResponsePost);
    } 
    else {
      String textResponseError = httpsPostDataClient.errorToString(httpCode).c_str();
      Serial.printf("Status is : %d : %s\n", httpCode, textResponseError);
      // deserializeJson(jsonPostDoc, textResponse1);
    }
    httpsPostDataClient.end(); // close the https connection;
  }
  else{
      Serial.printf("[HTTPS] Unable to connect to : %s", TOKEN_ENDPOINT);
  }

}
