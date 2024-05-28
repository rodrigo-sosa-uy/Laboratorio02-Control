#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "libraries/credentials.h"
#include <Servo.h>

// Defines para actuadores y sensores
#define Trigger D0
#define Echo D1
#define ENA D2
#define M1A D3                                                                                            
#define M1B D4
#define M2A D5
#define M2B D6
#define ServoDireccion D7
#define ServoSensor D8

//  Creación de objetos  //
ESP8266WiFiMulti WiFiMulti;
WiFiServer server(80);

Servo direccion;
Servo sensor;

//  Variables globales  //
String header;
unsigned int timeout = 2000;
unsigned long lastTime;
uint8_t velocidad = 220;
uint16_t servoDirPos = 95;
uint16_t servoSenPos = 95;
unsigned long t;
uint8_t d;

//  Declaración de funciones  //
void initWiFiMulti();
void etapaControl();
void PosicionInicial();
void getDistance();

void setup(){
  Serial.begin(9600);

  //  Inicialización de sensores y actuadores  //
  direccion.attach(ServoDireccion, 500, 2400); // Calibración para ESP8266 [5,95,180]
  sensor.attach(ServoSensor, 500, 2400); // Calibración para ESP8266 [15,95,180]

  pinMode(Trigger, OUTPUT);
  pinMode(Echo, OUTPUT);
  pinMode(ENA, OUTPUT); // LM1117 - 3,3 [MAX: 7V]
  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);

  //  Inicialización de WiFiMulti  //
  WiFiMulti.addAP(ssid_casa, pass_casa);
  WiFiMulti.addAP(ssid_telf, pass_telf);
  WiFiMulti.addAP(ssid_utec, pass_utec);

  initWiFiMulti();

  //  Inicialización de WiFiServer  //
  server.begin();

  delay(500);
  Serial.end();

  PosicionInicial();

  delay(2000);
}

void loop(){
  WiFiClient client = server.available();

  if(client){
    lastTime = millis();

    String currentLine = "";

    while(client.connected() && (millis() - lastTime <= timeout)){
      if(client.available()){
        char c = client.read();
        header += c;

        if(c == '\n'){
          if(currentLine.length() == 0){
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            etapaControl();
          } else{
            currentLine = "";
          }
        } else if(c != '\r'){
          currentLine += c;
        }
      }
    }
    header = "";
  } else{
    if(millis() >= (lastTime + 2500)){
      digitalWrite(M1A, 0);
      digitalWrite(M1B, 0);
      digitalWrite(M2A, 0);
      digitalWrite(M2B, 0);
    }
  }
}

void initWiFiMulti(){
  WiFi.mode(WIFI_STA);

  while(WiFiMulti.run(timeout) != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a la red.");
  Serial.print("SSID de Red: "); Serial.println(WiFi.SSID());
  Serial.print("Dirección IP: "); Serial.println(WiFi.localIP());
}

void etapaControl(){
  if(header.indexOf("GET /MOV=ADE") >= 0){
    servoDirPos = 95;
    direccion.write(servoDirPos);
    delay(25);

    analogWrite(ENA, velocidad);
    digitalWrite(M1A, 1);
    digitalWrite(M1B, 0);
    digitalWrite(M2A, 1);
    digitalWrite(M2B, 0);

  } else if(header.indexOf("GET /MOV=ATR") >= 0){
    servoDirPos = 95;
    direccion.write(servoDirPos);
    delay(25);
    
    analogWrite(ENA, velocidad);
    digitalWrite(M1A, 0);
    digitalWrite(M1B, 1);
    digitalWrite(M2A, 0);
    digitalWrite(M2B, 1);

  } else if(header.indexOf("GET /MOV=IZQ") >= 0){
    analogWrite(ENA, velocidad);
    digitalWrite(M1A, 1);
    digitalWrite(M1B, 0);
    digitalWrite(M2A, 1);
    digitalWrite(M2B, 0);

    servoDirPos = 25;
    direccion.write(servoDirPos);

  } else if(header.indexOf("GET /MOV=DER") >= 0){
    analogWrite(ENA, velocidad);
    digitalWrite(M1A, 1);
    digitalWrite(M1B, 0);
    digitalWrite(M2A, 1);
    digitalWrite(M2B, 0);

    servoDirPos = 165;
    direccion.write(servoDirPos);

  } else if(header.indexOf("GET /GIR=IZQ") >= 0){
    servoSenPos += 40;
    sensor.write(servoSenPos);

  } else if(header.indexOf("GET /GIR=DER") >= 0){
    servoSenPos -= 40;
    sensor.write(servoSenPos);

  } else if(header.indexOf("GET /POS=REIN") >= 0){
    PosicionInicial();

  } else if(header.indexOf("GET /VEL=MAX") >= 0){
    velocidad = 220;

  } else if(header.indexOf("GET /VEL=MED") >= 0){
    velocidad = 170;

  } else if(header.indexOf("GET /VEL=MIN") >= 0){
    velocidad = 100;
  }
}

void PosicionInicial(){
  servoDirPos = 95;
  servoSenPos = 95;
  direccion.write(servoDirPos);
  delay(10);
  sensor.write(servoSenPos);
  delay(10);

  digitalWrite(M1A, 0);
  digitalWrite(M1B, 0);
  digitalWrite(M2A, 0);
  digitalWrite(M2B, 0);
}

void getDistance(){
  digitalWrite(Trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);  //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  
  t = pulseIn(Echo, HIGH);  //obtenemos el ancho del pulso
  d = t / 58.4;             //escalamos el tiempo a una distancia en cm
}