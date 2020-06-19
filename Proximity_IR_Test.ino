#define IR_pin 7
int count=0, rpm_count,t,tim;
bool count_flag;
void steup()
{
  pinMode(IR_pin,INPUT);
}

void loop()
{
  if(digitalRead(IR_pin)==LOW)
  {
    count_flag = HIGH;
    tim = millis();
  }
  if(digitalRead(IR_pin)==HIGH &&  count_flag == HIGH)
  {
    count++;
    count_flag = LOW;
    t = millis()-tim;
  }
    rpm_count = (count*60000.0/t);
  
  Serial.println("Count=" + rpm_count);
}

