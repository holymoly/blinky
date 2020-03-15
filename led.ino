
// Programs available
typedef enum {
  MANUAL, 
  BALL,
  BLINK,
  RAINBOW
  } ledProgram_type;

ledProgram_type activeProgram = RAINBOW;

// initial colors
int red = 500;
int green = 50;
int blue = 50;

long cycleTime;

uint16_t rainbowIndex = 0;

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
        dimByFactorCycle(0.999, 0);
        
        // init Balls
        for(int i = 0; i <= nBalls; i++){
          pixels.setPixelColor(balls[i].moveTimeBased(), pixels.Color(balls[i].red, balls[i].green, balls[i].blue));
        }
      }
      break;
    case BLINK:
      {
        //  Uncomment one of these RGB (Red, Green, Blue) values to
        //  set the base color of the flame.  The color will flickr
        //  based on the initial base color
        
        //dimm pixels
        dimByFactorCycle(0.999, 5);
        
        //  Regular (orange) flame:
        int r = 226, g = 121, b = 35;
      
        //  Purple flame:
        //  int r = 158, g = 8, b = 148;
      
        //  Green flame:
        //int r = 74, g = 150, b = 12;
      
        //  Draw 10 stars
        if(cycleTime < millis()){
          cycleTime = millis() + 100;
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
    // Call Rainbow
    case RAINBOW:
      {
        if(cycleTime < millis()){
          cycleTime = millis() + 5;
          for(int i=0; i< pixels.numPixels(); i++){
            pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + rainbowIndex) & 255));
          }
          rainbowIndex++;
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

long nextDimTime = millis(); //inits nextDimTime dimByFactorCycle will be executed on first call

// dims all Leds by the factor if last dim was more than dimDiffTime ago
void dimByFactorCycle(float fadeFactor, int dimDiffTime){
  if(nextDimTime < millis()){
    nextDimTime = millis() + dimDiffTime; // set next dim cycle
    
    for (int i=0; i < NUMPIXELS; i++){   
      uint32_t curr_col = pixels.getPixelColor(i);
      uint8_t curr_b = curr_col & 0xFF;
      uint8_t curr_g = (curr_col >> 8) & 0xFF;
      uint8_t curr_r = (curr_col >> 16) & 0xFF;  // separate into RGB component
                 
      pixels.setPixelColor(i, pixels.Color(curr_r*fadeFactor,curr_g*fadeFactor,curr_b*fadeFactor));
    }
  }
}



//Rainbow color calculation
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85){
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else if(WheelPos < 170){
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  else{
    WheelPos -= 170;
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
