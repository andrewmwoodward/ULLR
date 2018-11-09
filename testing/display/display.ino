#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "graphics.h"

Adafruit_SSD1306 display = Adafruit_SSD1306();
 
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
}
 
int dataLabelNum = 0;
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
    
    display.setCursor(60,22);
    display.print("IMU data");
    
    display.display();
    
    
    delay(100); 
  }
}
