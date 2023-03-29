#include <Wire.h>

class accel{

    float smoothening_factor;
    float threshold;
    int address;
    float* accel_dat;

public:

    accel(int addr){
        address = addr;
        accel_dat = new float[3];
    }

    void setup(float sm_fact, float thresh){
        smoothening_factor = sm_fact;
        threshold = thresh;
        Wire.begin(); //Init wire
        Wire.write(0x2D); // Access talk to POWER_CTL Register: 0x2D
        Wire.write(8); //Bit D3 High for POWER_CTL
        Wire.endTransmission();
        delay(10);
    }

    void calibrate(){
        Wire.beginTransmission(address);
        Wire.write(0x1E); //X-Offset
        Wire.write(0);
        Wire.endTransmission();
        delay(10);

        Wire.beginTransmission(address);
        Wire.write(0x1F); //Y-Offset
        Wire.write(-8);
        Wire.endTransmission();
        delay(10);

        Wire.beginTransmission(address);
        Wire.write(0x20); //Z-Offset
        Wire.write(4);
        Wire.endTransmission();
        delay(10);
    }

    float* read(){
        Wire.beginTransmission(address);
        Wire.write(0x32); //Register 0x32: ACCEL_XOUT_H
        Wire.endTransmission(false);
        Wire.requestFrom(address,6,true); //read 6 registers
        accel_dat[0] = Wire.read()|Wire.read()<<8;
        accel_dat[1] = Wire.read()|Wire.read()<<8;
        accel_dat[2] = Wire.read()|Wire.read()<<8;
        
        //smoothening and threshold
        accel_dat[0] = accel_dat[0]/threshold;
        accel_dat[1] = accel_dat[1]/threshold;
        accel_dat[2] = accel_dat[2]/threshold;
        accel_dat[0] = (accel_dat[0] * smoothening_factor) + ((1-smoothening_factor) * accel_dat[0]);
        accel_dat[1] = (accel_dat[1] * smoothening_factor) + ((1-smoothening_factor) * accel_dat[1]);
        accel_dat[2] = (accel_dat[2] * smoothening_factor) + ((1-smoothening_factor) * accel_dat[2]);
        return accel_dat;
    }

    float roll(){
      read();
      return atan(accel_dat[1]/sqrt(pow(accel_dat[0],2)+pow(accel_dat[1],2)))*180/PI;
    }

    float pitch(){
      read();
      return atan((-1*accel_dat[0])/sqrt(pow(accel_dat[1],2)+pow(accel_dat[2],2)))*180/PI;
    }

};
