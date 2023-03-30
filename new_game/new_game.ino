#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#ifdef ADAFRUIT_HALLOWING
#define TFT_CS 39        // Hallowing display control pins: chip select
#define TFT_RST 37       // Display reset
#define TFT_DC 38        // Display data/command select
#define TFT_BACKLIGHT 7  // Display backlight pin

#elif defined(ARDUINO_FEATHER_ESP32)  // Feather Huzzah32
#define TFT_CS 14
#define TFT_RST 15
#define TFT_DC 32

#elif defined(ESP8266)
#define TFT_CS 4
#define TFT_RST 16
#define TFT_DC 5

#else
// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS 10
#define TFT_RST 9  // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 8
#endif


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
const uint8_t Button_pin = 2;

// color definitions
const uint16_t Display_Color_Black = 0x0000;
const uint16_t Display_Color_Blue = 0x001F;
const uint16_t Display_Color_Red = 0xF800;
const uint16_t Display_Color_Green = 0x07E0;
const uint16_t Display_Color_Cyan = 0x07FF;
const uint16_t Display_Color_Magenta = 0xF81F;
const uint16_t Display_Color_Yellow = 0xFFE0;
const uint16_t Display_Color_White = 0xFFFF;

#define PLATFORM_COLOR1 0x07E0
#define PLATFORM_COLOR2 0xFFE0
#define PLATFORM_COLOR3 0xF81F
#define PLATFORM_COLOR4 0xF800
#define BG_COLOR 0x0000
#define BALL_COLOR 0xF800

// class frame {
//   //class frames used for shifting the platforms and ball
//   int flag_need_frame_shift;

//   int check_frame_shift(ball b, platform p1, platform p2, platform p3) {

//     if (b.get_pos()[1] >= p3.get_pos()[1]) {
//       return 1;//start moving the frame
//     } else if(b.get_pos()[1]>=p1.get_pos()[1]){
//       return -1; //stop moving the frame
//     }

//     return 0; // continue with what is happening
//   }
// };


class platform {
  int* pos;
  int* size;
  int* vel;
  int color;
  int spike_begin;
  int spike_size;
public:

  void platform_init(int x, int y, int w, int h) {
    pos = new int[2];
    size = new int[2];
    vel = new int[2];
    pos[0] = x;
    pos[1] = y;
    size[0] = w;
    size[1] = h;
    vel[0] = -2;
    vel[1] = -5;
    spike_begin = 0;
    spike_size = 0;
    color = PLATFORM_COLOR1;
  }

  void set_spike(int start, int size) {
    spike_begin = start;
    spike_size = size;
  }

  int get_spike_start() {
    return pos[0] + spike_begin;
  }

  int get_spike_end() {
    return pos[0] + spike_begin + spike_size;
  }

  int* get_pos() {
    return pos;
  }

  int* get_size() {
    return size;
  }

  int get_color() {
    return color;
  }

  void update_vx(int val) {
    val = random(0, val + 1);
    vel[0] = (val - 3) / 3 + 1;
  }

  void update_vx_force(int val) {
    vel[0] = val;
  }

  void update_width(int val) {
    if (val % 2 == 0) {
      size[0] = size[0] - 5;
    }
    if (size[0] <= 30) {
      size[0] = 30;
    }
  }

  void update_py() {

    if (pos[1] < 20) {
      pos[1] = 116;
      pos[0] = random(0, 80);
    }
    pos[1] = (pos[1] + vel[1]);
  }

  void update_px() {
    if (pos[0] <= 0 || pos[0] + size[0] > 127) {
      size[0] = size[0] - 5;
      if (size[0] <= 30) {
        size[0] = 30;
      }
      vel[0] = -vel[0];
    }
    pos[0] = pos[0] + vel[0];
  }

  void set_color(int val) {
    color = val;
  }

  void draw() {
    tft.drawRect(pos[0], pos[1], size[0], size[1], color);
    for (int i = 0; i < spike_size && spike_begin + i < size[0]; i += 3) {
      tft.drawRect(pos[0] + spike_begin + i, pos[1] - 1, 4, 1, 0xF81F);
      tft.drawRect(pos[0] + spike_begin + i + 1, pos[1] - 2, 2, 1, 0xF81F);
    }
  }

  void erase() {
    tft.drawRect(pos[0], pos[1], size[0], size[1], BG_COLOR);
    for (int i = 0; i < spike_size && spike_begin + i < size[0]; i += 3) {
      tft.drawRect(pos[0] + spike_begin + i, pos[1] - 1, 4, 1, BG_COLOR);
      tft.drawRect(pos[0] + spike_begin + i + 1, pos[1] - 2, 2, 1, BG_COLOR);
    }
  }
};

class ball {
  int* pos;
  int radius;
  int* vel;
public:
  void ball_init(int x, int y, int rad) {
    radius = rad;
    pos = new int[2];
    vel = new int[2];
    pos[0] = x;
    pos[1] = y;
    vel[0] = 0.00;
    vel[1] = 0.00;
  }

  int* get_pos() {
    return pos;
  }

  void x_translate(int val) {
    pos[0] += val;
  }

  void y_translate(int val) {
    pos[1] += val;
  }

  int* get_vel() {
    return vel;
  }

  int get_radius() {
    return radius;
  }

  void update_px() {
    //Bounce Conditions
    if ((pos[0] <= 0 && vel[0] < 0) || (pos[0] >= 127 && vel[0] > 0)) {
      // vel[0] = -vel[0];
      vel[0] = 0;
    }
    pos[0] += vel[0];
  }

  void update_py() {
    //Bounce Conditions
    if ((pos[1] <= 0 && vel[1] < 0) || (pos[1] >= 127 && vel[1] > 0)) {
      // vel[1] = -vel[1];
      vel[1] = 0;
    }
    pos[1] += vel[1];
  }

  void update_vx(int x_accel) {
    vel[0] += x_accel;
  }

  void update_vy(int y_accel) {
    vel[1] += y_accel;
  }

  void stop_x() {
    vel[0] = 0;
  }

  void stop_y() {
    vel[1] = 0;
  }

  void erase() {
    tft.fillCircle(pos[0], pos[1], radius, BG_COLOR);
  }

  void draw() {
    tft.fillCircle(pos[0], pos[1], radius, BALL_COLOR);
  }

  void bounce_util(int val) {
    erase();
    y_translate(val);
    draw();
    delay(50);
  }

  void bounce() {
    bounce_util(-4);
    bounce_util(-2);
    bounce_util(-1);
    bounce_util(1);
    bounce_util(2);
    bounce_util(4);
  }
};


// ------------------------------------------GLOBAL VARIABLE DECLARATION-------------------------------
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();

ball b;
platform p1;
platform p2;
platform p3;
int score = 0;
int high_score = 0;
int coll;
char score_str[5] = { 0 };
float* store_accel;

int collision() {
  int *ball_pos, *ball_vel, *plat_pos, *plat_size;
  ball_pos = b.get_pos();
  ball_vel = b.get_vel();
  int rad = b.get_radius();

  plat_pos = p1.get_pos();
  plat_size = p1.get_size();
  int flag = 0;

  if ((ball_pos[1] + rad <= plat_pos[1]) && (ball_pos[1] + rad + ball_vel[1] >= plat_pos[1]) && (ball_pos[0] + rad >= plat_pos[0]) && (ball_pos[0] - rad <= plat_pos[0] + plat_size[0])) {

    if(p1.get_color() != PLATFORM_COLOR4){
      b.erase();
      b.stop_y();
      b.update_vy(plat_pos[1] - ball_pos[1] - rad);
      b.update_py();
      b.stop_y();
      Serial.println("C1");
      flag = 1;
    }
  }

  plat_pos = p2.get_pos();
  plat_size = p2.get_size();
  if ((ball_pos[1] + rad <= plat_pos[1]) && (ball_pos[1] + rad + ball_vel[1] >= plat_pos[1]) && (ball_pos[0] + rad >= plat_pos[0]) && (ball_pos[0] - rad <= plat_pos[0] + plat_size[0])) {

    if(p2.get_color() != PLATFORM_COLOR4){
      b.erase();
      b.stop_y();
      b.update_vy(plat_pos[1] - ball_pos[1] - rad);
      b.update_py();
      b.stop_y();
      Serial.println("C2");
      flag = 2;
    }
  }

  plat_pos = p3.get_pos();
  plat_size = p3.get_size();
  if ((ball_pos[1] + rad <= plat_pos[1]) && (ball_pos[1] + rad + ball_vel[1] >= plat_pos[1]) && (ball_pos[0] + rad >= plat_pos[0]) && (ball_pos[0] - rad <= plat_pos[0] + plat_size[0])) {

    if(p3.get_color() != PLATFORM_COLOR4){
      b.erase();
      b.stop_y();
      b.update_vy(plat_pos[1] - ball_pos[1] - rad);
      b.update_py();
      b.stop_y();
      Serial.println("C3");
      flag = 3;
    }
  }


  if (flag > 0) {
    if (coll == 0) {
      coll = 1;
      update_score();
      // b.bounce();
    }
    int start, end;
    if (flag == 1) {
      start = p1.get_spike_start();
      end = p1.get_spike_end();
      if (ball_pos[0] > start && ball_pos[0] < end) {
        game_over();
      }
    } else if (flag == 2) {
      start = p2.get_spike_start();
      end = p2.get_spike_end();
      if (ball_pos[0] > start && ball_pos[0] < end) {
        game_over();
      }
    } else {
      start = p3.get_spike_start();
      end = p3.get_spike_end();
      if (ball_pos[0] > start && ball_pos[0] < end) {
        game_over();
      }
    }

  } else if (flag == 0) {
    coll = 0;
  }
  return flag;
}


void render() {
  sensors_event_t event;
  accel.getEvent(&event);
  // Serial.print("X: ");
  // Serial.println(event.acceleration.x);

  if (score > 3) {
    if (p1.get_color() != PLATFORM_COLOR1) {
      p1.erase();
      p1.update_px();
      p1.draw();
    }
    if (p2.get_color() != PLATFORM_COLOR1) {
      p2.erase();
      p2.update_px();
      p2.draw();
    }
    if (p3.get_color() != PLATFORM_COLOR1) {
      p3.erase();
      p3.update_px();
      p3.draw();
    }
  }

  int col = collision();
  // b.update_vx(-event.acceleration.x/3);
  if (!col) {

    //TEMP
    b.update_vy(5);
    //
    // b.update_vy(store_accel[1]);
  }
  b.erase();
  p1.draw();
  p2.draw();
  p3.draw();
  b.x_translate(-event.acceleration.x * (min((score / 10) * 0.5 + 1.5, 3)));
  if (!col) {
    b.update_py();
  }
  b.draw();
  delay(100);
}


void moveup() {
  b.stop_x();
  b.stop_y();
  b.update_vy(-5.0);

  if (score > 9) {
    if (random(0, 100) < 30) {
      p1.set_color(PLATFORM_COLOR4);
    } else {
      p1.set_color(PLATFORM_COLOR2);
    }
    if (random(0, 100) < 30) {
      p2.set_color(PLATFORM_COLOR4);
    } else {
      p2.set_color(PLATFORM_COLOR2);
    }
  }

  else if (score > 4) {
    if (random(0, 100) % 4 > 0) {
      p1.set_color(PLATFORM_COLOR2);
    } else {
      p1.set_color(PLATFORM_COLOR1);
    }
    if (random(0, 100) % 4 < 2) {
      p2.set_color(PLATFORM_COLOR2);
    } else {
      p2.set_color(PLATFORM_COLOR1);
    }
  }
  int start;
  int size;
  if (score > 3) {
    if (random(0, 100) % 4 > 0) {
      size = min(score, (p1.get_size()[0] * 2) / 3);
      start = random(0, p1.get_size()[0] - size);
      p1.set_spike(start, size);
    }
    else{
      p1.set_spike(0, 0);
    }
    if (random(0, 100) % 4 < 2) {
      size = min(score, (p1.get_size()[0] * 2) / 3);
      start = random(0, p1.get_size()[0] - size);
      p2.set_spike(start, size);
    }
    else{
      p1.set_spike(0, 0);
    }
  }
  else{
    p1.set_spike(0, 0);
    p2.set_spike(0, 0);
    p3.set_spike(0, 0);
  }

  // if(score)

  while (b.get_pos()[1] + b.get_radius() >= 32) {
    p1.erase();
    p2.erase();
    p3.erase();
    b.erase();
    b.update_py();
    p1.update_py();
    p2.update_py();
    p3.update_py();

    b.draw();
    p1.draw();
    p2.draw();
    p3.draw();
    delay(100);
  }
  b.stop_y();
  b.update_vy(2.00);

  platform temp_p;
  temp_p = p2;
  p2 = p1;
  p1 = p3;
  p3 = temp_p;
}

void game_over() {

  tft.fillScreen(BG_COLOR);
  tft.setCursor(17, 60);
  tft.setTextSize(2);
  tft.print("GAME OVER");
  // Your Score
  tft.setCursor(30, 90);
  tft.setTextSize(1.2);
  tft.print("Your Score: ");
  sprintf(score_str, "%d", score);
  tft.print(score_str);

  if (score > high_score) {
    high_score = score;
  }

  tft.setCursor(30, 102);
  tft.setTextSize(1.5);
  tft.print("High Score: ");
  sprintf(score_str, "%d", high_score);
  tft.print(score_str);

  delay(2000);

  start_screen();

  tft.fillScreen(BG_COLOR);
  tft.setCursor(30, 0);
  tft.setTextSize(1.1);
  tft.setTextColor(0xFFFF);
  tft.print("Oopsy ");
  tft.setTextColor(BALL_COLOR);
  tft.print("Bombsy");
  tft.setTextColor(0xFFFF);
  b.ball_init(10, 0, 4);
  p3.platform_init(68, 116, 60, 2);
  p2.platform_init(38, 83, 60, 2);
  p1.platform_init(0, 42, 60, 2);
  b.draw();
  p1.draw();
  p2.draw();
  p3.draw();

  score = 0;
  tft.setCursor(70, 120);
  tft.setTextSize(0.3);
  tft.print("Score: ");
  sprintf(score_str, "%d", score);
  tft.print(score_str);
  coll = 0;

  b.update_vx(0.00);
  b.update_vy(2.00);
  delay(2000);
}

void update_score() {
  tft.setCursor(70, 120);
  tft.setTextColor(BG_COLOR);
  tft.setTextSize(0.3);
  tft.print("Score: ");
  sprintf(score_str, "%d", score);
  tft.print(score_str);

  score++;
  p1.update_vx(score);
  p2.update_vx(score);
  p3.update_vx(score);

  tft.setCursor(70, 120);
  tft.setTextColor(Display_Color_White);
  tft.setTextSize(0.3);
  tft.print("Score: ");
  sprintf(score_str, "%d", score);
  tft.print(score_str);
  // delay(100);
}

void start_screen() {
  tft.fillScreen(BG_COLOR);
  tft.setCursor(35, 50);
  tft.setTextSize(2);
  tft.print("OOPSY");
  tft.setCursor(30, 70);
  tft.setTextColor(BALL_COLOR);
  tft.print("BOMBSY");
  delay(2000);
}

void setup() {
  randomSeed(analogRead(A0));
  Serial.begin(9600);
  if (!accel.begin()) {
    Serial.println("No ADXL345 sensor detected.");
    while (1)
      ;
  }
#ifdef ADAFRUIT_HALLOWING
  tft.initR(INITR_HALLOWING);  // Initialize HalloWing-oriented screen
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);  // Backlight on
#else
  tft.initR(INITR_144GREENTAB);  // Init ST7735R chip, green tab
#endif
  start_screen();
  tft.fillScreen(BG_COLOR);
  tft.setCursor(30, 0);
  tft.setTextSize(1.1);
  tft.setTextColor(0xFFFF);
  tft.print("Oopsy ");
  tft.setTextColor(BALL_COLOR);
  tft.print("Bombsy");
  tft.setTextColor(0xFFFF);
  score = 0;
  tft.setCursor(70, 120);
  tft.setTextSize(0.3);
  tft.print("Score: ");
  sprintf(score_str, "%d", score);
  tft.print(score_str);
  coll = 0;

  b.ball_init(10, 0, 4);
  p3.platform_init(random(0, 80), 116, 60, 2);
  p2.platform_init(random(0, 80), 83, 60, 2);
  p1.platform_init(0, 42, 60, 2);
  b.draw();
  p1.draw();
  p2.draw();
  p3.draw();
  b.update_vx(1.00);
  b.update_vy(2.00);
  delay(500);
}
// SETUP FUNCTION ENDS HERE

void loop() {
  // put your main code here, to run repeatedly:
  int get_col = collision();
  // Serial.println("%d", get_col);
  if (get_col == 3) {
    //This function is called me the ball reaches the bottom of the screen
    moveup();
  } else if (b.get_pos()[1] + b.get_radius() > 125) {
    game_over();
  } else {
    render();
  }
  // if(b.get_vel()[1]<0){
  //   if(b.get_vel()[1]>-2){
  //     b.update_vy(-b.get_vel()[1]);
  //   }
  //   else{
  //     b.update_vy(-2);
  //   }
  // }
  // b.update_vx(1);
  // b.update_vy(2);
}
