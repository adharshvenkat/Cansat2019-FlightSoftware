#include<SoftwareSerial.h>
#include<XBee.h>
#include<EEPROM.h>
int x,y,z;

SoftwareSerial XBee(1,0);
void setup() {
  z = 20;
  x = 0;
  y = 0;
  XBee.begin(9600);
  Serial.begin(9600);

}

void loop() {
  y = EEPROM.read(2);
  if (Serial.available())
  { 
    x = Serial.read();
  if (x == 84){
    Serial.print("Calibration is done!");
    EEPROM.put(2,21);
    y = EEPROM.read(2); 
  }
  }
  if ( y == 21 &&  z ==20){
    Serial.print("Hello");
  }
  delay(500);
}
