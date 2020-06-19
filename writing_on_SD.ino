#include <SD.h>
int a;
float b=34.67;
String str="teststr";

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
 
  myFile = SD.open("test.txt", FILE_WRITE);
 
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.print(a);
    myFile.print(",");
    myFile.print(b);
    myFile.print(",");
    myFile.print(str);
    myFile.print(";");
    myFile.println("");
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening test.txt");
  }
}
 
void loop()
{
  // nothing happens after setup
}
