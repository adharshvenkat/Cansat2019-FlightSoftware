#define SERVO_PIN 5

void setup()
{
  
}
void loop() 
{
        digitalWrite(SERVO_PIN, HIGH);
        delayMicroseconds(2000); // The range of the pulse is between 1000µs till 2000µs (from 0 till 180degrees)
        digitalWrite(SERVO_PIN, LOW);
        delayMicroseconds(18000); // This will result into a period. NOTE: 18500+1500=20000µs=20ms which is the period
        
        digitalWrite(SERVO_PIN, HIGH);
        delayMicroseconds(1000); // The range of the pulse is between 1000µs till 2000µs (from 0 till 180degrees)
        digitalWrite(SERVO_PIN, LOW);
        delayMicroseconds(19000); // This will result into a period. NOTE: 18500+1500=20000µs=20ms which is the period

}
