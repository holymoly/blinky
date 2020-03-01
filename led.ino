
// Programs available
typedef enum {
  MANUAL, 
  BALL,
  LAUFLICHT,
  RAINBOW,
  BLINK
  } ledProgram_type;

ledProgram_type activeProgram = BLINK;

// initial colors
int red = 500;
int green = 50;
int blue = 50;
int speed = 50;

long starTime;

static void chase(uint32_t c) {
  for(uint16_t i=0; i<pixels.numPixels()+4; i++) {
    pixels.setPixelColor(i  , c); // Draw new pixel
    pixels.setPixelColor(i-4, 0); // Erase pixel a few steps back
    pixels.show();
    delay(50);
  }
}
void colorWipe(int red, int green, int blue, int SpeedDelay) {
  for(uint16_t i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, red, green, blue);
      pixels.show();
      delay(SpeedDelay);
  }
}
void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
       pixels.setPixelColor(i, *c, *(c+1), *(c+2));
    }
     pixels.show();
    delay(SpeedDelay);
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

void setLeds(){
  switch (activeProgram) {
    case MANUAL:
      {
        //Colors for rgb will be set from websocket
        for (int i=0; i<NUMPIXELS; i++){              
          pixels.setPixelColor(i, pixels.Color(red,green,blue));
        }
      }
      break;
    // Call the ball program 
    case BALL:
      {
        //dimm pixels
        float fadeFactor = 0.999;
        for (int i=0; i < NUMPIXELS; i++){   
          uint32_t curr_col = pixels.getPixelColor(i);
          uint8_t curr_b = curr_col & 0xFF;
          uint8_t curr_g = (curr_col >> 8) & 0xFF;
          uint8_t curr_r = (curr_col >> 16) & 0xFF;  // separate into RGB component
                     
          pixels.setPixelColor(i, pixels.Color(curr_r*fadeFactor,curr_g*fadeFactor,curr_b*fadeFactor));
        }
        // init Balls
        for(int i = 0; i <= nBalls; i++){
          pixels.setPixelColor(balls[i].moveTimeBased(), pixels.Color(balls[i].red, balls[i].green, balls[i].blue));
        }
      }
      break;
      case CHASE:
       chase(pixels.Color(255, 0, 0)); // Red
       chase(pixels.Color(0, 255, 0)); // Green
       chase(pixels.Color(0, 0, 255)); // Blue
      
    break;
    case RAINBOW:
      rainbowCycle(speed);
    break;
    case LAUFLICHT:
    colorWipe(red,green,blue,speed);
    break;
    case BLINK:
      {
        //  Uncomment one of these RGB (Red, Green, Blue) values to
        //  set the base color of the flame.  The color will flickr
        //  based on the initial base color
        
        //dimm pixels
        float fadeFactor = 0.999;
        for (int i=0; i < NUMPIXELS; i++){   
          uint32_t curr_col = pixels.getPixelColor(i);
          uint8_t curr_b = curr_col & 0xFF;
          uint8_t curr_g = (curr_col >> 8) & 0xFF;
          uint8_t curr_r = (curr_col >> 16) & 0xFF;  // separate into RGB component
                     
          pixels.setPixelColor(i, pixels.Color(curr_r*fadeFactor,curr_g*fadeFactor,curr_b*fadeFactor));
        }
        
        //  Regular (orange) flame:
        int r = 226, g = 121, b = 35;
      
        //  Purple flame:
        //  int r = 158, g = 8, b = 148;
      
        //  Green flame:
        //int r = 74, g = 150, b = 12;
      
        //  Draw 10 stars
        if(starTime < millis()){
          starTime = millis() + 100;
          int flicker = random(0,55);
          int r1 = r-flicker;
          int g1 = g-flicker;
          int b1 = b-flicker;
          if(g1<0) g1=0;
          if(r1<0) r1=0;
          if(b1<0) b1=0;
          pixels.setPixelColor(random(0,pixels.numPixels()),r1,g1, b1);
        }
      }
      break;
    default:
      {
        //Switch red if no mode was found
        for (int i=0; i<NUMPIXELS; i++){              
          pixels.setPixelColor(i, pixels.Color(20,0,0));
        }
      }
  }
}
