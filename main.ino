#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "libraries/credentials.h"
#include <Servo.h>

// Defines para actuadores y sensores
#define Trigger D0
#define Echo D1
#define ENA D2
#define MA D3
#define MB D4
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
uint8_t velocidad = 255;
uint16_t servoDirPos = 1750;
uint16_t servoSenPos = 1500;
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
  direccion.attach(ServoDireccion, 750, 2750); // Calibración para ESP8266
  sensor.attach(ServoSensor, 650, 2400); // Calibración para ESP8266

  pinMode(Trigger, OUTPUT);
  pinMode(Echo, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(MA, OUTPUT);
  pinMode(MB, OUTPUT);

  //  Inicialización de WiFiMulti  //
  WiFiMulti.addAP(ssid_casa, pass_casa);
  WiFiMulti.addAP(ssid_telf, pass_telf);
  WiFiMulti.addAP(ssid_utec, pass_utec);

  initWiFiMulti();

  //  Inicialización de WiFiServer  //
  server.begin();

  //Serial.end();

  PosicionInicial();

  delay(2000);
}

void loop(){
  WiFiClient client = server.available();

  if(client){
    lastTime = millis();

    Serial.println("Nuevo Cliente.");
    String currentLine = "";

    while(client.connected() && (millis() - lastTime <= timeout)){
      if(client.available()){
        char c = client.read();
        Serial.write(c);
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
    Serial.println("Movimiento ADELANTE");
    Serial.print("Velocidad: "); Serial.println(velocidad);
    analogWrite(ENA, velocidad);
    digitalWrite(MA, 1);
    digitalWrite(MB, 0);

  } else if(header.indexOf("GET /MOV=ATR") >= 0){
    Serial.println("Movimiento ATRAS");
    Serial.print("Velocidad: "); Serial.println(velocidad);
    analogWrite(ENA, velocidad);
    digitalWrite(MA, 0);
    digitalWrite(MB, 1);

  } else if(header.indexOf("GET /MOV=IZQ") >= 0){
    Serial.println("Movimiento IZQUIERDA");
    Serial.print("Velocidad: "); Serial.println(velocidad);
    analogWrite(ENA, velocidad);
    digitalWrite(MA, 1);
    digitalWrite(MB, 0);

    servoDirPos += 250;
    direccion.writeMicroseconds(servoDirPos);
    Serial.print("Angulo Direccion: "); Serial.println(servoDirPos);

  } else if(header.indexOf("GET /MOV=DER") >= 0){
    Serial.println("Movimiento DERECHA");
    Serial.print("Velocidad: "); Serial.println(velocidad);
    analogWrite(ENA, velocidad);
    digitalWrite(MA, 1);
    digitalWrite(MB, 0);

    servoDirPos -= 250;
    direccion.writeMicroseconds(servoDirPos);
    Serial.print("Angulo Direccion: "); Serial.println(servoDirPos);

  } else if(header.indexOf("GET /GIR=IZQ") >= 0){
    Serial.println("Giro IZQUIERDA");
    servoSenPos += 250;
    sensor.writeMicroseconds(servoSenPos);
    Serial.print("Angulo Sensor: "); Serial.println(servoSenPos);

  } else if(header.indexOf("GET /GIR=DER") >= 0){
    Serial.println("Giro DERECHA");
    servoSenPos -= 250;
    sensor.writeMicroseconds(servoSenPos);
    Serial.print("Angulo Sensor: "); Serial.println(servoSenPos);

  } else if(header.indexOf("GET /POS=REIN") >= 0){
    Serial.println("Reinicio de posición");
    PosicionInicial();

  } else if(header.indexOf("GET /VEL=MAX") >= 0){
    Serial.println("Velocidad MAXIMA");
    velocidad = 255;

  } else if(header.indexOf("GET /VEL=MED") >= 0){
    Serial.println("Velocidad MEDIA");
    velocidad = 140;

  } else if(header.indexOf("GET /VEL=MIN") >= 0){
    Serial.println("Velocidad MINIMA");
    velocidad = 40;
  }
}

void PosicionInicial(){
  servoDirPos = 1750;
  servoSenPos = 1500;
  direccion.writeMicroseconds(servoDirPos);
  delay(10);
  sensor.writeMicroseconds(servoSenPos);
  delay(10);

  Serial.print("Angulo Direccion: "); Serial.println(servoDirPos);
  Serial.print("Angulo Sensor: "); Serial.println(servoSenPos);

  digitalWrite(MA, 0);
  digitalWrite(MB, 0);
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