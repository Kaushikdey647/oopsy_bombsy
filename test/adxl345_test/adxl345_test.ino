#include "accel.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

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

const uint8_t  Button_pin = 2;

accel adxl345(0x53);

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

void setup() {
  pinMode(Button_pin,INPUT_PULLUP);
  delay(250);
  Serial.begin(9600);
  adxl345.setup(0.2, 1.00);
  adxl345.calibrate();
#ifdef ADAFRUIT_HALLOWING
  tft.initR(INITR_HALLOWING);  // Initialize HalloWing-oriented screen
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);  // Backlight on
#else
  tft.initR(INITR_144GREENTAB);  // Init ST7735R chip, green tab
#endif
  tft.fillScreen(BG_COLOR);
}

int ball_x=64;
int ball_y=64;
int ball_radius=7;


int wi=60;
int hi=2;

int i = 0;


void platforms(int x, int y,int width, int height, uint16_t color){
  tft.fillRect(x,  y,  width,  height,color);
}

void render(){
  float* vals = adxl345.read();
  float a_x = vals[0];
  float a_y = vals[1];
  Serial.print(a_x);
  Serial.print(" ");
  Serial.println(a_y);
  //update ball position
  ball_x += (a_x/200);
  ball_x %= 128;
  ball_y += (a_y/200);
  ball_y %= 128;

  //fill circle
  tft.fillCircle(ball_x,ball_y, ball_radius, BALL_COLOR);

}

void loop() {
  render();
	delay(500);
}
