#include<SoftwareSerial.h>
#include<EEPROM.h>

uint32_t timer;
int m_time, m_count,pct,pkt,count,count2;

void setup() {
  Serial.begin(9600);

}

void loop() {
  if (millis()- timer >1000)
  {
  m_count = EEPROM.read(8);
  count = EEPROM.read(9);
  pct = EEPROM.read(10);
  count2 = EEPROM.read(11);
  
  if(pct <=254)
  {
    pct = pct + 1;
  }
  else
  {
    count2 = count2 + 1;
    pct = 0;
  }
  if(m_count <=254)
  {
    m_count = m_count +((millis() - timer)/1000);
  }
  else
  {
    count = count + 1;
    m_count = 0;
  }
  EEPROM.put(8,m_count);
  EEPROM.put(9,count);
  EEPROM.put(10,pct);
  EEPROM.put(11,count2);
  m_time = 255*count + m_count;
  pkt = 255*count2 + pct;
  timer = millis();
  Serial.print("5601");
  Serial.print(",");
  Serial.print(m_time);
  Serial.print(",");
  Serial.print(pkt);
  Serial.print(";");
  Serial.print("270.18");
  Serial.print(",");
  Serial.print("1013.25");
  Serial.print(",");
  Serial.print("34.25");
  Serial.print(",");
  Serial.print("2.5");
  Serial.print(",");
  Serial.print("0:0:0");
  Serial.print(",");
  Serial.print("0.0000");
  Serial.print(",");
  Serial.print("0.0000");
  Serial.print(",");
  Serial.print("0.0");
  Serial.print(",");
  Serial.print("0");
  Serial.print(",");
  Serial.print("21.26");
  Serial.print(",");
  Serial.print("67.21");
  Serial.print(",");
  Serial.print("0");
  Serial.print(",");
  Serial.print("G");
  Serial.print(",");
  Serial.print("231.45");
  Serial.print(";");
  Serial.println();
  }
  
}






