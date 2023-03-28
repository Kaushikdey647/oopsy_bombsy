/*
 * This is an example sketch that shows how to toggle the display
 * on and off at runtime to avoid screen burn-in.
 * 
 * The sketch also demonstrates how to erase a previous value by re-drawing the 
 * older value in the screen background color prior to writing a new value in
 * the same location. This avoids the need to call fillScreen() to erase the
 * entire screen followed by a complete redraw of screen contents.
 * 
 * Originally written by Phill Kelley. BSD license.
 * Adapted for ST77xx by Melissa LeBlanc-Williams
 */

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

#define SerialDebugging true

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

// For 1.44" and 1.8" TFT with ST7735 (including HalloWing) use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// For 1.3", 1.54", and 2.0" TFT with ST7789:
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// OPTION 2 lets you interface the display using ANY TWO or THREE PINS,
// tradeoff being that performance is not as fast as hardware SPI above.
//#define TFT_MOSI 11  // Data out
//#define TFT_SCLK 13  // Clock out
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// connect a push button between ground and...
const uint8_t   Button_pin              = 2;

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

    // button press pulls pin LOW so configure HIGH
    pinMode(Button_pin,INPUT_PULLUP);

    // use an interrupt to sense when the button is pressed

    #if (SerialDebugging)
    Serial.begin(115200); while (!Serial); Serial.println();
    #endif

    // settling time
    delay(250);

    // ignore any power-on-reboot garbage
   

    #ifdef ADAFRUIT_HALLOWING
      // HalloWing is a special case. It uses a ST7735R display just like the
      // breakout board, but the orientation and backlight control are different.
      tft.initR(INITR_HALLOWING);        // Initialize HalloWing-oriented screen
      pinMode(TFT_BACKLIGHT, OUTPUT);
      digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight on
    #else
      // Use this initializer if using a 1.8" TFT screen:
      // tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
    
      // OR use this initializer (uncomment) if using a 1.44" TFT:
      tft.initR(INITR_144GREENTAB); // Init ST7735R chip, green tab
    
      // OR use this initializer (uncomment) if using a 0.96" 180x60 TFT:
      //tft.initR(INITR_MINI160x80);  // Init ST7735S mini display
    
      // OR use this initializer (uncomment) if using a 1.54" 240x240 TFT:
      //tft.init(240, 240);           // Init ST7789 240x240

      // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
      //tft.init(240, 320);           // Init ST7789 320x240
  
      // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
      // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
      // may end up with a black screen some times, or all the time.
      //tft.setSPISpeed(40000000);
    #endif

    // initialise the display
    tft.fillScreen(BG_COLOR);
    tft.setCursor(20, 0);
    tft.setTextSize(1.1);
    tft.print("Platformers");
}

// int platformsxx[4][4]=[[0,0,0,0],
//                     [0,0,0,0],
//                     [0,0,0,0],
//                     [0,0,0,0]];

int ball_x=10;
int ball_radius=7;


int wi=60;
int hi=2;

int i = 0;

int ball_y=128-ball_radius-hi;

void platforms(int x, int y,int width, int height, uint16_t color){
  tft.fillRect(x,  y,  width,  height,color);
}



void loop() {
    tft.setTextColor(Display_Color_Black);
    tft.fillCircle(ball_x,ball_y, ball_radius, BG_COLOR);
    platforms(0,128-hi-i,wi,hi, Display_Color_Black);
    platforms(128-wi,128-hi-40-i,wi,hi, Display_Color_Black);
    platforms(0,128-hi-80-i,wi,hi, Display_Color_Black);
    if(ball_x>128-ball_radius){
      ball_x=10;
    }
    ball_x+=1;
    i += 1;
    // tft.fillCircle(ball_x,ball_y, ball_radius, BALL_COLOR);
    platforms(0,128-hi-i,wi,hi, PLATFORM_COLOR);
    platforms(128-wi,128-hi-40-i,wi,hi, PLATFORM_COLOR);
    platforms(0,128-hi-80-i,wi,hi, PLATFORM_COLOR);

    // tft.setTextColor(Display_Color_Black);
    // tft.fillCircle(ball_x,ball_y, ball_radius, BG_COLOR);
    // if(ball_x>128-ball_radius){
    //   ball_x=10;
    // }
    // ball_x+=1;
    // i += 1;
    // tft.fillCircle(ball_x,ball_y, ball_radius, BALL_COLOR);
    ';'
    // platforms(0,128-hi,wi,hi, PLATFORM_COLOR);
    // platforms(128-wi,128-hi-40,wi,hi, PLATFORM_COLOR);
    // platforms(0,128-hi-80,wi,hi, PLATFORM_COLOR);
    delay(40);
}
