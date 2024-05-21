#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "credentials.h"
#include <Servo.h>

// Defines para actuadores y sensores
#define ServoDireccion Dx
#define ServoSensor Dx

//  Creación de objetos  //
ESP8266WiFiMulti WiFiMulti;
WiFiServer server(80);

Servo direccion;
Servo sensor;

//  Variables globales  //
String header;
unsigned long lastTime,timeout = 2000;
unsigned char AngleDireccion = 90;
unsigned char AngleSensor = 90;

//  Declaración de funciones  //
void initWiFiMulti();
void etapaControl();
void PosicionInicial();

void setup() {
  Serial.begin(9600);

  //  Inicialización de sensores y actuadores  //
  direccion.attach(ServoDireccion);
  sensor.attach(ServoSensor);

  //  Inicialización de WiFiMulti  //
  WiFiMulti.addAP(ssid_casa, pass_casa);
  WiFiMulti.addAP(ssid_telf, pass_telf);
  WiFiMulti.addAP(ssid_utec, pass_utec);

  initWiFiMulti();

  //  Inicialización de WiFiServer  //
  server.begin();
}

void loop() {
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
    // Logica para adelante.

  } else if(header.indexOf("GET /MOV=ATR") >= 0){
    Serial.println("Movimiento ATRAS");
    // Logica para atras.

  } else if(header.indexOf("GET /MOV=IZQ") >= 0){
    Serial.println("Movimiento IZQUIERDA");
    // Logica para izquierda.

  } else if(header.indexOf("GET /MOV=DER") >= 0){
    Serial.println("Movimiento DERECHA");
    // Logica para derecha.

  } else if(header.indexOf("GET /GIR=IZQ") >= 0){
    Serial.println("Giro IZQUIERDA");
    // Logica para giro izquierda.

  } else if(header.indexOf("GET /GIR=DER") >= 0){
    Serial.println("Giro DERECHA");
    // Logica para giro derecha.

  } else if(header.indexOf("GET /POS=REIN") >= 0){
    Serial.println("Reinicio de posición");
    PosicionInicial();

  } else if(header.indexOf("GET /VEL=MAX") >= 0){
    Serial.println("Velocidad MAXIMA");
    // Logica para setear velocidad maxima.

  } else if(header.indexOf("GET /VEL=MED") >= 0){
    Serial.println("Velocidad MEDIA");
    // Logica para setear velocidad media.

  } else if(header.indexOf("GET /VEL=MIN") >= 0){
    Serial.println("Velocidad MINIMA");
    // Logica para setear velocidad minima.

  }
}

void PosicionInicial(){

}