#include <Wire.h>

int adxl = 0x53;

float a_x, a_y, a_z; //g-value

float thres = 1.00; //threshold

float e = 0.1; //history factor

void setup() {
  Serial.begin(9600); //begin console comms
  Wire.begin(); //Init wire
  Wire.write(0x2D); // Access talk to POWER_CTL Register: 0x2D
  Wire.write(8); //Bit D3 High for POWER_CTL
  Wire.endTransmission();
  delay(10);

  //CALIBRATION

  Wire.beginTransmission(adxl);
  Wire.write(0x1E); //X-Offset
  Wire.write(0);
  Wire.endTransmission();
  delay(10);

  Wire.beginTransmission(adxl);
  Wire.write(0x1F); //Y-Offset
  Wire.write(-8);
  Wire.endTransmission();
  delay(10);

  Wire.beginTransmission(adxl);
  Wire.write(0x20); //Z-Offset
  Wire.write(4);
  Wire.endTransmission();
  delay(10);

}

void loop() {
  Wire.beginTransmission(adxl);
  Wire.write(0x32); //Register 0x32: ACCEL_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom(adxl,6,true); //read 6 registers
  a_x = (a_x * e) + ((1-e) * (Wire.read()|Wire.read()<<8)/thres);
  a_y = (a_y * e) + ((1-e) * (Wire.read()|Wire.read()<<8)/thres);
  a_z = (a_z * e) + ((1-e) * (Wire.read()|Wire.read()<<8)/thres);

  Serial.print(x);
  Serial.print(" | ");
  Serial.print(y);
  Serial.print(" | ");
  Serial.println(z);
}