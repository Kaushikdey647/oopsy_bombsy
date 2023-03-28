#include "accel.h"

accel adxl345(0x53);

void setup() {
  Serial.begin(9600);
  adxl345.setup(0.2, 1.00);
  adxl345.calibrate();
}

void loop() {
	float* vals = adxl345.read();
	Serial.print("X: ");
	Serial.print(vals[0]);
	Serial.print(" Y: ");
	Serial.print(vals[1]);
	Serial.print(" Z: ");
	Serial.println(vals[2]);
	delay(100);
}
