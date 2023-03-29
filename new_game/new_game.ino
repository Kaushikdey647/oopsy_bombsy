#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include "accel.h"

#ifdef ADAFRUIT_HALLOWING
  #define TFT_CS        39 // Hallowing display control pins: chip select
  #define TFT_RST       37 // Display reset
  #define TFT_DC        38 // Display data/command select
  #define TFT_BACKLIGHT  7 // Display backlight pin

#elif defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
  #define TFT_CS         14
  #define TFT_RST        15
  #define TFT_DC         32

#elif defined(ESP8266)
  #define TFT_CS         4
  #define TFT_RST        16                                            
  #define TFT_DC         5

#else
  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
  #define TFT_CS        10
  #define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         8
#endif


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
const uint8_t Button_pin = 2;

// color definitions
const uint16_t  Display_Color_Black        = 0x0000;
const uint16_t  Display_Color_Blue         = 0x001F;
const uint16_t  Display_Color_Red          = 0xF800;
const uint16_t  Display_Color_Green        = 0x07E0;
const uint16_t  Display_Color_Cyan         = 0x07FF;
const uint16_t  Display_Color_Magenta      = 0xF81F;
const uint16_t  Display_Color_Yellow       = 0xFFE0;
const uint16_t  Display_Color_White        = 0xFFFF;

#define PLATFORM_COLOR 0x07E0
#define BG_COLOR 0x0000
#define BALL_COLOR 0xF800

class ball{
  int* pos;
  int radius;
  float* vel;
  public:
  ball(int x, int y, int rad){
    radius = rad;
    pos = new int[2];
    vel = new float[2];
    pos[0] = x;
    pos[1] = y;
  }
  
  int* get_pos(){
    return pos;
  }

  float* get_vel(){
    return vel;
  }

  void update_px(){
    //Bounce Conditions
    if(( pos[0] <= 0 && vel[0] < 0 ) || ( pos[0] >= 127 && vel[0] > 0 )){
      vel[0] = -vel[0];
    }
    pos[0] += vel[0];
  }
  
  void update_py(){
    //Bounce Conditions
    if(( pos[0] <= 0 && vel[0] < 0 ) || ( pos[0] >= 127 && vel[0] > 0 )){
      vel[0] = -vel[0];
    }
    pos[1] += vel[1];
  }

  void update_vx(float x_accel){
    vel[0] += x_accel;
  }

  void update_vy(float y_accel){
    vel[1] += y_accel;
  }

  void stop_x(){
    vel[0] = 0;
  }

  void stop_y(){
    vel[1] = 0;
  }

  void erase(){
    tft.fillCircle(pos[0], pos[1], radius, BG_COLOR);
  }

  void draw(){
    tft.fillCircle(pos[0], pos[1], radius, BALL_COLOR);
  }
  
};

ball b(64,64,4);

accel adxl345(0x53);

float* store_accel = new float[3];

void render(){
  store_accel = adxl345.read();
  b.update_vx(store_accel[0]);
  b.update_vy(store_accel[1]);
  b.erase();
  b.update_px();
  b.update_py();
  b.draw();
  delay(75);
}

void setup() {
    delay(250);
    #ifdef ADAFRUIT_HALLOWING
      tft.initR(INITR_HALLOWING);        // Initialize HalloWing-oriented screen
      pinMode(TFT_BACKLIGHT, OUTPUT);
      digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight on
    #else
      tft.initR(INITR_144GREENTAB); // Init ST7735R chip, green tab
    #endif
    tft.fillScreen(BG_COLOR);
    tft.setCursor(30, 0);
    tft.setTextSize(1.1);
    tft.print("Platformers");

    Serial.begin(9600);
    adxl345.setup(0.2, 1.00);
    adxl345.calibrate();
}

void loop() {
  // put your main code here, to run repeatedly:
  render();
}
