#pragma once
#define M5STACK_MPU6886
#include <Arduino.h>
#include <M5Stack.h>

class Motion {
public:

bool update()
{
  if(micros() - imu_last >= imu_time) //25hz is hard coded
  {
    imu_last = micros();
    static float r,p,yaw,x,y; // ,z;
    M5.IMU.getAhrsData(&p, &r, &yaw);
    x = sin(p * DEG_TO_RAD);
    y = -cos(p * DEG_TO_RAD) * sin(r * DEG_TO_RAD);
    //z = cos(r * DEG_TO_RAD) * cos(p * DEG_TO_RAD);
    deg = atan2(y,-x) * RAD_TO_DEG;
    // Serial.printf("%5.2f, %5.2f, %5.2f, %5.2f\n", x, y, z, atan2(y,x) * RAD_TO_DEG);
    return true;
  }
  return false;
}

float getDegree()
{
  return deg;
}


private:
  const uint32_t imu_time = 40000;
  unsigned long  imu_last;
  float deg;
};