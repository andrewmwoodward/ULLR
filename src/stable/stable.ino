#include <SPI.h>
#include <Wire.h>
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

// create a pointer array so that the bitmaps can be called in sequence easily
const unsigned char* animation[7] = {ullr1, ullr2, ullr3, ullr4, ullr5, ullr6, ullr7};

const char *dataLabelString[5] = {"Jump","360", "540", "Moguls", "Backflip"};

 
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
    display.clearDisplay();
    display.drawBitmap(0, 1, animation[i], 51, 29, WHITE);
    
    display.setCursor(60,2);
    if (! digitalRead(BUTTON_A)){
      dataLabelNum++;
      if(dataLabelNum==5) dataLabelNum = 0; // ensure it wraps around
    }
    display.print(dataLabelString[dataLabelNum]);

    display.setCursor(60,12);
    if (! digitalRead(BUTTON_B)){
      if(isRecording){
        display.print("Start");
        isRecording = false;  
      }else{
        display.print("Stop REC");
        isRecording = true;  
      }
    }else{
      if(isRecording){
        display.print("Stop REC");
      }else{
        display.print("Start");
      }
    }
    

    // last line of displays IMU data
    display.setCursor(60,22);
    IMU.readSensor();

    int value1, value2, value3;
    if(! digitalRead(BUTTON_C)){
      dataIMUNum++;
      if(dataIMUNum==3) dataIMUNum = 0;
    }

    switch(dataIMUNum){
      case 0:
        // display the accel data
        value1 = round(IMU.getAccelX_mss());
        value2 = round(IMU.getAccelY_mss());
        value3 = round(IMU.getAccelZ_mss());
        break;
      
      case 1:
        // display the gyro data
        value1 = round(IMU.getGyroX_rads());
        value2 = round(IMU.getGyroY_rads());
        value3 = round(IMU.getGyroZ_rads());
        break;

      case 2:
        // display the magnetometer data
        value1 = round(IMU.getMagX_uT());
        value2 = round(IMU.getMagY_uT());
        value3 = round(IMU.getMagZ_uT());
        break;
    }
    
    display.print(value1);
    display.print(" ");
    display.print(value2);
    display.print(" ");
    display.print(value3);
    
    display.display();
    
    
    delay(100); 
  }
}
