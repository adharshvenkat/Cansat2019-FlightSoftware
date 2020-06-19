#include<SoftwareSerial.h>

SoftwareSerial XBee(10,11);
void setup()
{
  XBee.begin(9600);
  Serial.begin(9600);
}

void loop()
{
    XBee.write("Cunt,FO Godwyn");
    Serial.print("CUNT");
    delay(1000);
    
    
}


