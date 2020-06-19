#include <SD.h>
 
File myFile;
 
void setup()
{
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
   pinMode(10, OUTPUT);
 
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
 
  if (SD.exists("test.txt")) {
    SD.remove("test.txt");
    
  }
}
 
void loop()
{
  // nothing happens after setup
}
