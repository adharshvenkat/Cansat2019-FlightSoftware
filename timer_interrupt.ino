boolean toggle1 = 0;

void setup() {
  pinMode(13, OUTPUT);

  TCCR2A = 0;// set entire TCCR2A register to 0
TCCR2B = 0;// same for TCCR2B
TCNT2 = 0;//initialize counter value to 0
// set compare match register for 8khz increments
OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)
// turn on CTC mode
TCCR2A |= (1 << WGM21);
// Set CS21 bit for 8 prescaler
TCCR2B |= (1 << CS21);
// enable timer compare interrupt
TIMSK2 |= (1 << OCIE2A);

sei();//allow interrupts

}
ISR(TIMER2_COMPA_vect){//timer1 interrupt 8kHz toggles pin 9
// =============================================================
// generates pulse wave of frequency 8kHz/2 = 4kHz (takes two cycles for full wave)
if (toggle1){
digitalWrite(13,HIGH);
toggle1 = 0;
}
else{
digitalWrite(13,LOW);
toggle1 = 1;
}
}


void loop() {
  
}
