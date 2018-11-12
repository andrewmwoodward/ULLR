/*
  SD card datalogger

  This example shows how to log data from three randomly generated numbers
  to an SD card using the SD library.

  Incorporates the A button on the oled feather board for turning on and off data logging

*/

#include <SPI.h>
#include <SD.h>

#define BUTTON_A 9

const int chipSelect = 4; // this is the default on the adafruit feather datalogger

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  // initialize button pin
  pinMode(BUTTON_A, INPUT_PULLUP);

  randomSeed(123455234);
}



bool flag = false;
int fileCount = 1;

void loop() {

  if (!digitalRead(BUTTON_A) && flag==false) {
    flag = true;
    fileCount = fileCount+1;
  }else if(!digitalRead(BUTTON_A) && flag==true){
    flag = false;
  }
  
  if (flag == true) {
    // make a string for assembling the data to log:
    String dataString = "";
    // get three random numbers to be saved
    for (int i = 0; i < 3; i++) {
      int num = random(1,10);
      dataString += String(num);
      if (i < 2) {
        dataString += ",";
      }
    } 

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    String fileString = "datalog" + String(fileCount) + ".txt";
    File dataFile = SD.open(fileString, FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      // print to the serial port too:
      Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
  }else{
    Serial.println("no data generated/saved");
  }
  delay(200);
}
