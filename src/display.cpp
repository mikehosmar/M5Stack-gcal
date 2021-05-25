#include "display.h"

Display::Display(bool repeat_frame)
{
  twice = repeat_frame;
}

void Display::setup()
{
  M5.Lcd.fillScreen(GREEN);
  createDialScale();
  createNeedle();
}

void Display::update()
{
  frame_count++;
  
  if(micros() - frame_count_last >= frame_count_time)
  {
    frame_count_last = micros();
    fps = (uint32_t)(frame_count* (float)1000000 / frame_count_time);
    frame_count = 0;
  }

  if (micros() - tft_last >= tft_time || twice)
  {
    // Serial.println(dt);
    tft_last = micros();
    M5.Lcd.setTextSize(2); 
    M5.Lcd.setCursor(0,0);
    M5.Lcd.setTextColor(BLACK, DARKGREEN);
    M5.Lcd.print(fps);
    M5.Lcd.setCursor(320,0);
    M5.Lcd.print(M5.Power.getBatteryLevel());
    
    // plotDial(160 - 45, 0, 1.0 / ((double)dt / 1000000ULL) - 120, "FPS", 1.0 / ((double)dt / 1000000ULL));
    // twice = !twice;
    // M5.Lcd.setPivot(320/2, 240/2);
    // static float deg_disp;
    // Serial.printf("%d\n", frames_since_imu);
    // deg_disp = ((deg1 - deg0) * (static_cast<float>(tft_time / imu_time)) * frames_since_imu) + deg0;  // linear interpolate
    plotDial(320/2-80, 240/2-80, degree - 90, "", 0);
    
    frames_since_imu++;
  }
}

void Display::setDegree(float degrees)
{
  degree = degrees;
  frames_since_imu = 0;
}

// =======================================================================================
// Create the dial sprite, the dial outer and place scale markers
// =======================================================================================

void Display::createDialScale()
{
  // Create the dial Sprite
  dial.setColorDepth(8);     // Size is odd (i.e. 91) so there is a centre pixel at 45,45
  dial.createSprite(171, 171); // 8bpp requires 91 * 91 = 8281 bytes
  dial.setPivot(86, 86);     // set pivot in middle of dial Sprite

  // Draw dial outline
  dial.fillSprite(TFT_TRANSPARENT);          // Fill with transparent colour
  dial.fillCircle(86, 86, 80, TFT_BLUE); // Draw dial outer
}

// =======================================================================================
// Add the empty dial face with label and value
// =======================================================================================

void Display::drawEmptyDial(String label, int16_t val)
{
  // Draw black face
  dial.fillCircle(86, 86, 80, TFT_DARKGREEN);
  dial.drawPixel(86, 86, TFT_WHITE); // For demo only, mark pivot point with a while pixel

  dial.setTextDatum(TC_DATUM); // Draw dial text
  //dial.drawString(label, 81, 30, 2);
  //dial.drawNumber(val, 81, 120, 2);
}

// =======================================================================================
// Update the dial and plot to screen with needle at defined angle
// =======================================================================================

void Display::plotDial(int16_t x, int16_t y, int16_t angle, String label, uint16_t val)
{
  // Draw the blank dial in the Sprite, add label and number
  drawEmptyDial(label, val);

  // Push a rotated needle Sprite to the dial Sprite, with black as transparent colour
  needle.pushRotated(&dial, angle, TFT_GREEN); // dial is the destination Sprite

  // Push the resultant dial Sprite to the screen, with transparent colour
  dial.pushSprite(x, y, TFT_TRANSPARENT);
}

// =======================================================================================
// Create the needle Sprite and the image of the needle
// =======================================================================================

void Display::createNeedle(void)
{
  needle.setColorDepth(8);
  needle.createSprite(17, 99); // create the needle Sprite 23 pixels wide by 99 high

  needle.fillSprite(TFT_GREEN); // Fill with black

  // Define needle pivot point
  uint16_t piv_x = needle.width() / 2;   // x pivot of Sprite (middle)
  uint16_t piv_y = needle.height() - 20; // y pivot of Sprite (20 pixels from bottom)
  needle.setPivot(piv_x, piv_y);         // Set pivot point in this Sprite

  // Draw the red needle with a yellow tip
  // Keep needle tip 1 pixel inside dial circle to avoid leaving stray pixels
  needle.fillRect(piv_x - 2, 2, 5, piv_y + 16, TFT_RED);
  needle.fillRect(piv_x - 2, 2, 5, 10, TFT_YELLOW);

  // Draw needle centre boss
  needle.fillCircle(piv_x, piv_y, 7, TFT_MAROON);
  needle.fillCircle(piv_x, piv_y, 1, TFT_WHITE); // Mark needle pivot point with a white pixel
}

// =======================================================================================