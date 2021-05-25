#pragma once
#define M5STACK_MPU6886
#include <Arduino.h>
#include <M5Stack.h>

class Display{
public:
Display(bool repeat_frame);

void setup();
void update();

void setDegree(float degrees);

private:
void createDialScale();
void drawEmptyDial(String label, int16_t val);
void plotDial(int16_t x, int16_t y, int16_t angle, String label, uint16_t val);
void createNeedle(void);

TFT_eSprite dial = TFT_eSprite(&M5.Lcd);
TFT_eSprite needle = TFT_eSprite(&M5.Lcd);

unsigned long tft_last, frame_count_last;
uint32_t frame_count, fps;
uint16_t frames_since_imu;
float degree;

const uint32_t frame_count_time = 200000;
const uint32_t tft_time = 20000;

bool twice;
};