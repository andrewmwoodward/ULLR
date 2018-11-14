#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "graphics.h"
#include "MPU9250.h"

Adafruit_SSD1306 display = Adafruit_SSD1306();

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);
 
#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5
#define LED      13

 
#if (SSD1306_LCDHEIGHT != 32)
 #error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// for SD card communication
const int chipSelect = 4; // this is the default on the adafruit feather datalogger

// create a pointer array so that the bitmaps can be called in sequence easily
const unsigned char* animation[7] = {ullr1, ullr2, ullr3, ullr4, ullr5, ullr6, ullr7};
const unsigned char* animation2[12] = {jump1, jump2, jump3, jump4, jump5, jump6, jump7, jump8, jump9, jump10, jump11, jump12};

String dataLabelString[5] = {"Jump","360", "540", "Moguls", "Backflip"};

 
void setup() {  
  Serial.begin(9600);
 
  Serial.println("OLED FeatherWing test");
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  Serial.println("OLED begun");
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);
 
  // Clear the buffer.
  display.clearDisplay();
  display.display();
  
  Serial.println("IO test");
 
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  // text display setup (not sure if this needs to be called after clearDisplay in loop)
  display.setTextSize(1);
  display.setTextColor(WHITE);

  
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  
  // start communication with IMU 
  int status = IMU.begin();
  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while(1) {}
  }

  
}
 
int dataLabelNum = 0;
int dataIMUNum = 0;
bool isRecording = false;
 
void loop() {
  for(int i=0; i<7; i++){
    // clear the display
    display.clearDisplay();

    // draw the i'th frame of the bitmap image
    display.drawBitmap(0, 1, animation[i], 51, 29, WHITE);

    // this displays the classification category
    display.setCursor(60,2);
    if (! digitalRead(BUTTON_A)){
      dataLabelNum++;
      if(dataLabelNum==5) dataLabelNum = 0; // ensure it wraps around
    }
    display.print(dataLabelString[dataLabelNum]);


    // get and display IMU data
    display.setCursor(60,22);
    IMU.readSensor();
    
    int xAccel = IMU.getAccelX_mss();
    int yAccel = IMU.getAccelY_mss();
    int zAccel = IMU.getAccelZ_mss();

    int xGyro = IMU.getGyroX_rads();
    int yGyro = IMU.getGyroY_rads();
    int zGyro = IMU.getGyroZ_rads();

    int xMag = IMU.getMagX_uT();
    int yMag = IMU.getMagY_uT();
    int zMag = IMU.getMagZ_uT();
    
    if(! digitalRead(BUTTON_C)){
      dataIMUNum++;
      if(dataIMUNum==3) dataIMUNum = 0;
    }
    // display either accel/gyro/mag data
    switch(dataIMUNum){
      case 0:
        // display the accel data
        display.print("A ");
        display.print(round(xAccel));
        display.print(" ");
        display.print(round(yAccel));
        display.print(" ");
        display.print(round(zAccel));
        break;
      
      case 1:
        // display the gyro data
        display.print("G ");
        display.print(round(xGyro));
        display.print(" ");
        display.print(round(yGyro));
        display.print(" ");
        display.print(round(zGyro));
        break;

      case 2:
        // display the magnetometer data
        display.print("M ");
        display.print(round(xMag));
        display.print(" ");
        display.print(round(yMag));
        display.print(" ");
        display.print(round(zMag));
        break;
    }

    // now if data needs to be saved save to the sd card files
    String sdFileName = dataLabelString[dataLabelNum] + ".txt";
    File dataFile = SD.open(sdFileName, FILE_WRITE);

    // create the string to save
    String data = String(xAccel) + "," + String(yAccel) + "," + String(zAccel) + "," +
                  String(xGyro)  + "," + String(yGyro)  + "," + String(zGyro)  + "," +
                  String(xMag)   + "," + String(yMag)   + "," + String(zMag);
    
    display.setCursor(60,12);
    if (! digitalRead(BUTTON_B)){
      if(isRecording){
        display.print("Start");
        isRecording = false;
        // need to signify that this specific recording is over
        dataFile.println("end");
      }else{
        display.print("Stop REC");
        isRecording = true;
        // starting a new recording
        dataFile.println("start");
        dataFile.println(data);    
      }
    }else{
      if(isRecording){
        display.print("Stop REC");
        // append data to the file
        dataFile.println(data);  
      }else{
        display.print("Start");
      }
    }
    dataFile.close(); // need to close the file

    // finally update the display
    display.display();
    
    
    delay(100); 
  }
  
  
}
