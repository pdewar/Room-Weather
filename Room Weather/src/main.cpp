#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "password.h"
#include "sound.h"

#define buzzer 25  //Buzzer pin

#define LED_PIN 33

#define DHTPIN 32     

#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

int pubscanTime = 3000; //default publish interval in milliseconds
unsigned long pubtime_now = 0;

bool alarmstatus = false;
bool ledstatus = false;

int humidThreshold = 100;       //default 100 initialized from node red
int tempThreshold = 100;        //default 100 initialized from node red

const char* mqtt_server = "test.mosquitto.org";
#define mqtt_port 1883
#define MQTT_SERIAL_PUBLISH_CH "csc113/serialdata/student15/publish"
#define MQTT_SERIAL_SUBSCRIBE_CH "csc113/serialdata/student15/subscribe/+"

WiFiClient wifiClient;

PubSubClient client(wifiClient);

void setup_wifi() {
  delay(10);
  //We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  //Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    //Attempt to connect
    //if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) 
    if (client.connect(clientId.c_str()) )
    {
      Serial.println("connected");
      //subscribe
      client.subscribe(MQTT_SERIAL_SUBSCRIBE_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      //Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void alertSOUND(){ 
  if(alarmstatus == true){
    tone(buzzer, 1000, 1000);
  }
}

void alertLED(){
  if(ledstatus == true){
    digitalWrite(LED_PIN, HIGH);     
  }
  digitalWrite(LED_PIN, LOW);
}

void testALERT(){
  digitalWrite(LED_PIN, HIGH);
  tone(buzzer, 1000, 3000);
  digitalWrite(LED_PIN, LOW); 
}

void callback(char* topic, byte *payload, unsigned int length) {
  Serial.println("-------new message from broker-----");
  Serial.print("channel:");
  Serial.println(topic);
  Serial.print("data:");  
  Serial.write(payload, length);
  Serial.println();
  
  if(String(topic) == "csc113/serialdata/student15/subscribe/led"){
    String temp_payload;
    for(int i = 0; i< length; i++){
    temp_payload += (char)payload[i];
    }
    if(temp_payload == "LED:Activated")
      ledstatus = true; //enable LED
    else
      ledstatus = false; //disable LED
  }
  else if(String(topic) == "csc113/serialdata/student15/subscribe/alert"){
    String temp_payload;
    for(int i = 0; i< length; i++){
    temp_payload += (char)payload[i];
    }
    if(temp_payload == "Alarm:Activated")
      alarmstatus = true; //enable alarm
    else
      alarmstatus = false; //disable alarm
  }
  else if(String(topic) == "csc113/serialdata/student15/subscribe/temp"){
    char payload_string[length + 1];
    int temp_payload;

    memcpy(payload_string, payload, length);
    payload_string[length] = '\0';
    temp_payload = atoi(payload_string);

    tempThreshold = temp_payload; //set threshold to new value
  }
  else if(String(topic) == "csc113/serialdata/student15/subscribe/humid"){
    char payload_string[length + 1];
    int temp_payload;

    memcpy(payload_string, payload, length);
    payload_string[length] = '\0';
    temp_payload = atoi(payload_string);
    
    humidThreshold = temp_payload; //set threshold to new value
  }
  else if(String(topic) == "csc113/serialdata/student15/subscribe/test"){
    testALERT(); 
  }
}

void publishSerialData(const char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  Serial.println(String("publishing: ") + serialData);
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(buzzer, OUTPUT);
  pinMode(LED_PIN,OUTPUT);
  Serial.println("before dht begin");
  dht.begin();
  
  Serial.setTimeout(500);//Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
  Serial.setTimeout(5000);
}

void loop() {
  client.loop();

  if(millis() > pubtime_now + pubscanTime){
    //Read humidity
    int humidity_measurement = round(dht.readHumidity()); 
    //Read temperature as Celsius (the default)
    int celcius_measurement = round(dht.readTemperature());
    //Read temperature as Fahrenheit (isFahrenheit = true)
    int fahrenheit_measurement = round(dht.readTemperature(true));
  
    StaticJsonDocument<300> doc;

    doc["device"] = "ESP32";

    JsonArray Fahrenheit = doc.createNestedArray("Fahrenheit");

    JsonArray Celcius = doc.createNestedArray("Celcius");

    JsonArray Humidity = doc.createNestedArray("Humidity");
 
    Fahrenheit.add(fahrenheit_measurement);
    Celcius.add(celcius_measurement);
    Humidity.add(humidity_measurement);
 
    char JSONmessageBuffer[100]; //json doc

    serializeJson(doc, JSONmessageBuffer); //write json doc to char 
    publishSerialData(JSONmessageBuffer);  //publish the json char
    
    if(humidity_measurement > humidThreshold || fahrenheit_measurement > tempThreshold){
      alertSOUND();
      alertLED();
      if(ledstatus == true){
      digitalWrite(LED_PIN, HIGH);     
      }
    }
    else{
      digitalWrite(LED_PIN, LOW);
    }

    //wait approx. [period] ms
    pubtime_now = millis();
  }

}