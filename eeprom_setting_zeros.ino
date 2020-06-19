#include<EEPROM.h>

int i=0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  while(i<EEPROM.length()){
    EEPROM.write(i,0);
    i+=1;
  }
}

