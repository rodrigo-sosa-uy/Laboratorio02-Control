#include <Servo.h> 

Servo myservo;  //creamos un objeto servo

void setup(){
  myservo.attach(3);  // asignamos el pin 3 al servo.
  Serial.begin(9600); // iniciamos el puerto serial
}
 
void loop(){ 
  myservo.write(0);
  Serial.println("Ángulo: 0°");
  delay(2000);  

  myservo.write(90);
  Serial.println("Ángulo: 90°");
  delay(2000); 

  myservo.write(180);
  Serial.println("Ángulo: 180°");
  delay(2000); 

  myservo.write(90);
  Serial.println("Ángulo: 90°");
  delay(2000); 
} 