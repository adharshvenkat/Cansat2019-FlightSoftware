int trig = 4;

void setup() {                
  // initialize the digital pins as output.
  pinMode(trig, OUTPUT);

  digitalWrite(trig, HIGH); 

}


void loop() {
  digitalWrite(trig, LOW);
  
  delay(1200);               

  digitalWrite(trig, HIGH);

  delay(10000);               
  
}
