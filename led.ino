
// Programs available
typedef enum {
  MANUAL, 
  BALL,
  FIRE,
  RAINBOW,
  BLINK
  } ledProgram_type;

ledProgram_type activeProgram = BLINK;

// initial colors
int red = 500;
int green = 50;
int blue = 50;

long starTime;

static void chase(uint32_t c) {
  for(uint16_t i=0; i<pixels.numPixels()+4; i++) {
    pixels.setPixelColor(i  , c); // Draw new pixel
    pixels.setPixelColor(i-4, 0); // Erase pixel a few steps back
    pixels.show();
    delay(50);
  }
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
      case RAINBOW:
       chase(pixels.Color(255, 0, 0)); // Red
       chase(pixels.Color(0, 255, 0)); // Green
       chase(pixels.Color(0, 0, 255)); // Blue
      
    break;
    case FIRE:
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
