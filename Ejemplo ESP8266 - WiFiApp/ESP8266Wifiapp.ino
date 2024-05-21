#include <ESP8266WiFi.h>

#define ssid "Rodriagus"
#define password "coquito15"

WiFiServer server(80);

String header;

unsigned long lastTime,timeout = 2000;
String outputState = "Apagado";

void setup() {
  Serial.begin(9600);

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  initWiFi();

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

            if(header.indexOf("GET /MOV=DER") >= 0){
              Serial.println("LED encendido");
              outputState = "Encendido";
              digitalWrite(BUILTIN_LED, HIGH);
            } else if(header.indexOf("GET /MOV=IZQ") >= 0){
              Serial.println("LED apagado");
              outputState = "Apagado";
              digitalWrite(BUILTIN_LED, LOW);
            }
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

void initWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a la red.");
  Serial.print("Dirección IP: "); Serial.println(WiFi.localIP());
}
