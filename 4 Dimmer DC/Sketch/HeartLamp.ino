
#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>


//ssid e password per connettersi
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* broker = "192.168.0.2";
const char* nome = "HeartLamp";  //inserire il nome del client

int perc = 0;
int perprec = 0;
int ledstato = 2;
//numero pin rele' 1
int channel1 = 4;
//numero pin pulsante 1
int sensorPin1 = 5;
bool up = HIGH;
String numero;


// Topic in uscita1
String outTopic = "CasaRage/HeartLamp/stato";
String inTopic = "CasaRage/HeartLamp";

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient net;
MQTTClient client;

void setup() {
  pinMode(ledstato, OUTPUT);
  digitalWrite(ledstato, LOW);
  pinMode(channel1, OUTPUT);
  pinMode(sensorPin1, INPUT);
  Serial.begin(9600);
  Serial.println("start");
  Serial.println("imposto modalità wifiSTA (client)");
  WiFi.mode(WIFI_STA);
  Wirelesscon();
  pagina();
  client.onMessage(messageReceived);
}

void loop() {

  if (numero != "") {
    perc = numero.toInt();
    perprec = perc;
    Serial.println("la percentuale è" + perc);
    numero = "";
    setlamp();
  }
  
  if (digitalRead(sensorPin1) == HIGH) {
    delay(300);
    if (digitalRead(sensorPin1) == HIGH) {
      while (digitalRead(sensorPin1) == HIGH) {
        client.loop();
        int valore = map(perc, 0, 100, 0, 255);
        analogWrite(channel1, valore);
        delay(20);
        if (up == HIGH) {
          perc++;
          if (perc > 99) up = LOW;
        }
        if (up == LOW) {
          perc--;
          if (perc < 1) up = HIGH;
        }
      }
      if (perprec != perc) setlamp();
      perprec = perc;
    } else {
      if (perc > 0) {
        perc = 0;
        setlamp();
      } else {
        if (perprec == 0) perprec = 100;
        perc = perprec;
        setlamp();
      }
    }
  }
  vercon();
  client.loop();
  httpServer.handleClient();
  MDNS.update();
}



void messageReceived(String& topic, String& payload) {
  Serial.println(topic + payload);
  perc = payload.toInt();
  perprec = perc;
  Serial.println("la percentuale è" + perc);

  setlamp();
}

void setlamp() {
  int valore = map(perc, 0, 100, 0, 255);
  analogWrite(channel1, valore);
  Serial.print("pubblico stato lampada a:");
  Serial.println(perc);
  if (client.connected()) {
    client.publish(outTopic, String(perc));
    Serial.println("pubblico percentuale reale: ");
    Serial.println(perc);
  }
}
