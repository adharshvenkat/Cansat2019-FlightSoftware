#include <SPI.h>
#include "SparkFunBME280.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include<SoftwareSerial.h>
#include<XBee.h>
#include<EEPROM.h>
#include <Adafruit_GPS.h>
#define degconvert 57.2957786
#define Buzzer_pin A0
#define GPSECHO false    
#define IR_pin A3
#define SERVO_PIN 5


float AcX,AcY,AcZ,MgX,MgY,roll,pitch,heading,volti,current_level,maximum_altitude,previous_level,delta,alt,cal_alt,cal_roll,cal_pitch,curr_GPS_alt,cal_GPS_alt,GPS_alt;
boolean telem = 0;
static float Pi = 3.14159;
int m_time, m_count,pct,pkt,count,count2,pos = 0,cam_trigger = 4,rpm = 0,x = 0,y = 0,z = 20,activated_flag = 1,previous_state = 0,release_flag = 0,rpm_man = 200;
int d = 0;
int Pin1 = 6; 
int Pin2 = 7; 
int Pin3 = 8; 
int Pin4 = 9; 
int _step = 0;
unsigned long elapsed_prev = 0,elapsed = 0,duration = 0;
uint32_t timer;
char ss,state,c;


Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);


BME280 mysensor;


SoftwareSerial mySerial(3,2);
SoftwareSerial XBee(1,0);

Adafruit_GPS GPS(&mySerial);

void useInterrupt(boolean);


void setup()
{
  cli(); // Disabling all interrupts

//------------ Telemetry Interrupt -----------------------------------!!!!  
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1 = 0; // initialize counter value to 0

  // set compare match register for 1hz increments
  OCR1A = 8500;

  // turn on CTC mode
  TCCR1B |= (1 << WGM12);

  //  Set CS12 and CS10 bits for 256 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);

  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();
//---------------------------------------------------------------------!!!!

  
  pinMode(Pin1, OUTPUT);  
  pinMode(Pin2, OUTPUT);  
  pinMode(Pin3, OUTPUT);  
  pinMode(Pin4, OUTPUT);  
  pinMode(A1,INPUT);
  pinMode(cam_trigger,OUTPUT);
  pinMode(IR_pin,INPUT);
  digitalWrite(cam_trigger,HIGH);
  Serial.begin(9600);
  XBee.begin(9600);
  mysensor.beginSPI(10);
  accel.begin();
  mag.begin();  
  mag.enableAutoRange(true);
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);  //setting update rate to 1 Hz
  useInterrupt(true);
  delay(10);

}

// 1 Hz timer interrupt
ISR(TIMER1_COMPA_vect)
{
  if (telem)
  {
    telemetry();
    telem = 0;
    }
    else
    {
      telem = 1;
    }
}

// GPS Interrupt

SIGNAL(TIMER0_COMPA_vect) {
  c = GPS.read();
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    }
}


void loop()
{

//  Calibration Command 
  if (Serial.available())
  {
    x = Serial.read();
    if (x == 84)
    {
      //Insert Calibration Command
      EEPROM.put(2,21);
      cal_roll = atan2(AcY, AcZ)*degconvert;
      cal_pitch = atan2(-AcX, AcZ)*degconvert;
      cal_alt = mysensor.readFloatAltitudeMeters();
      cal_GPS_alt = GPS.altitude;

      EEPROM.put(8,0);
      EEPROM.put(9,0);
      EEPROM.put(10,0);
      EEPROM.put(11,0);
      EEPROM.put(15,0);
      //---------------- Camera deployment -----------------------------------------------------------------------------------
      digitalWrite(cam_trigger, LOW);
      delay(1200);               
      digitalWrite(cam_trigger, HIGH);
//-----------------------------------------------------------------------------------------------------------------------

    for (int i = 0; i < 100; i += 1)
      { 
        digitalWrite(SERVO_PIN, HIGH);
        delayMicroseconds(1700); // The range of the pulse is between 1000µs till 2000µs (from 0 till 180degrees)
        digitalWrite(SERVO_PIN, LOW);
        delayMicroseconds(17700); // This will result into a period. NOTE: 18500+1500=20000µs=20ms which is the period
      }
    }
    if(x == 70)
    {
      for (int i = 0; i < 100; i += 1)
      { 
        digitalWrite(SERVO_PIN, HIGH);
        delayMicroseconds(2800); // The range of the pulse is between 1000µs till 2000µs (from 0 till 180degrees)
        digitalWrite(SERVO_PIN, LOW);
        delayMicroseconds(18800); // This will result into a period. NOTE: 18500+1500=20000µs=20ms which is the period
      } 
    }
  }
  
  
  y = EEPROM.read(2);
  release_flag = EEPROM.read(15);
  //d = EEPROM.read(16);
 
// Function Calls
  altitude();
  software_state();
  release_mechanism();
  rpm_calculation();

  EEPROM.put(15,release_flag);
  
//Data acquiring and calculation
  volti = analogRead(A1);
  volti = volti*(5.0/1024.0);
  
  sensors_event_t event;
  accel.getEvent(&event);
  mag.getEvent(&event);
  AcX = event.acceleration.x;
  AcY = event.acceleration.y;
  AcZ = event.acceleration.z;
  MgX = event.magnetic.x;
  MgY = event.magnetic.y;
  roll = atan2(AcY, AcZ)*degconvert;
  pitch = atan2(-AcX, AcZ)*degconvert;
  heading = (atan2(MgY,MgX)*180)/Pi;
  
  alt = mysensor.readFloatAltitudeMeters();
  GPS_alt = GPS.altitude;
  if (heading <0 )
  {
    heading = 360 + heading;
  }
   
 while(!GPS.newNMEAreceived()) 
{
  c=GPS.read(); 
}
  GPS.parse(GPS.lastNMEA());  

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  
         timer = millis();
}


void telemetry()
{
  if (y == 21 && z == 20)
  {
  
  y = EEPROM.read(2);
  m_count = EEPROM.read(8);
  count = EEPROM.read(9);
  pct = EEPROM.read(10);
  count2 = EEPROM.read(11);
  state = EEPROM.read(12);
  if(pct <= 254)
  {
    pct = pct + 1;
    
  }
  else
  {
    count2 = count2 + 1;
    pct = 0;
  }
  if(m_count <= 254)
  {
    m_count = m_count + ((millis() - timer)/1000);
    timer = millis();
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
  Serial.print("5601");
  Serial.print(",");
  Serial.print(m_time);
  Serial.print(",");
  Serial.print(pkt);
  Serial.print(",");
  Serial.print(alt - cal_alt);
  Serial.print(",");
  Serial.print(mysensor.readFloatPressure());
  Serial.print(",");
  Serial.print(mysensor.readTempC());
  Serial.print(",");
  Serial.print(volti);
  Serial.print(",");
  Serial.print(GPS.hour, DEC); Serial.print(':');
  Serial.print(GPS.minute, DEC); Serial.print(':');
  Serial.print(GPS.seconds, DEC); 
  Serial.print(",");
  Serial.print(GPS.latitudeDegrees, 4);
  Serial.print(","); 
  Serial.print(GPS.longitudeDegrees, 4);
  Serial.print(",");
  Serial.print(curr_GPS_alt, 1); 
  Serial.print(",");
  Serial.print((int)GPS.satellites);
  Serial.print(",");
  Serial.print((pitch - cal_pitch)*24);
  Serial.print(",");
  Serial.print((roll - cal_roll)*24);
  Serial.print(",");
  Serial.print(rpm);
  Serial.print(",");
  Serial.print(state);
  Serial.print(",");
  Serial.print(heading);
  Serial.print(";");
  Serial.println();
    } 
}

int altitude()
{
  current_level = (alt - cal_alt);
  curr_GPS_alt = (GPS_alt - cal_GPS_alt);
  if ((alt - cal_alt) < 500.0)
  {
    if (maximum_altitude < current_level)
    {
      maximum_altitude = current_level;
    }
  }
  else
  {
    release_flag = 1;
  }    
}


int software_state()
{
  delta = current_level - previous_level;
  previous_level = current_level;

  if (release_flag == 0)
  {
    if (delta <=1.0 && delta >=-1.0)
    {
    ss = 'G';
    }
   else if (delta >=1.0)
    {
      ss = 'A';
      rpm = 0;
    }
    else if (delta <=-1.0)
    {
      ss = 'D';
      rpm = 0;
    }
  }
  else if (release_flag == 1)
  {
    if (((current_level < 700.0) && (curr_GPS_alt < 700.0)) && ((current_level > 40.0) &&(curr_GPS_alt > 40.0)))
    {
      ss = 'P';
      rpm  = rpm_man + 1;
    }
    else if ((current_level < 40.0) && (curr_GPS_alt < 40.0))
    {
      ss = 'L';
      rpm = 0;
    }
  }
  EEPROM.put(12,ss);
}

int release_mechanism()
{
  if (state == 'P')
  {
    if(d == 0){     


//------------------------Servo Release Mechanism Trigger----------------------------------------------------------------
   for (int i = 0; i < 100; i += 1)
      { 
        digitalWrite(SERVO_PIN, HIGH);
        delayMicroseconds(2800); // The range of the pulse is between 1000µs till 2000µs (from 0 till 180degrees)
        digitalWrite(SERVO_PIN, LOW);
        delayMicroseconds(18800); // This will result into a period. NOTE: 18500+1500=20000µs=20ms which is the period
      } 
//-----------------------------------------------------------------------------------------------------------------------
      d = d + 1; 
      //EEPROM.put(16,d);
    }
  }
           
    if (state == 'L')
    {    
    //Audio Beacon activation, Camera stop & Telemetry Stop
      analogWrite(Buzzer_pin,255);
      digitalWrite(cam_trigger,LOW);
      EEPROM.write(2,0);
    }
}


int rpm_calculation()
{
  if(state == 'P')
  {
    if (digitalRead(IR_pin) == 1 && previous_state == 0)
    {
      previous_state = 1;
      duration = elapsed - elapsed_prev;
      elapsed_prev  = micros();    
    }
    if (digitalRead(IR_pin) == 1 && previous_state == 1)
    {
      previous_state = 1;       
    }
    if (digitalRead(IR_pin) == 0 && previous_state == 1)
    {
      previous_state = 0;     
    }
    if (digitalRead(IR_pin) == 0 && previous_state == 0)
    {
      previous_state = 0;
      elapsed = micros();    
    }
    rpm = (60000000/duration)/2;
  }
  else
  {
    rpm = 0;
  }
}
