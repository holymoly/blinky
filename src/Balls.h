class Balls {
   public:
      int actPosition;   
      int startPosition;
      int endPosition;
      uint positionHoldPeriod;
      int speed;
      int gravitation;
      byte red;
      byte green;
      byte blue;
      byte tailLength;
      bool direction;
      
      Balls(int _positionHoldPeriod, byte _red, byte _green, byte _blue, byte _tailLength, int _startPosition, int _endPosition);
      Balls(int _speed, int _gravitation, byte _red, byte _green, byte _blue, byte _tailLength);
      Balls();
      int moveTimeBased();
      int moveAgainstGravitation();
   private:
      long unsigned int lastMoveTick;
      long unsigned int startTime;
};


//Constructor for time based movement
Balls::Balls(int _positionHoldPeriod, byte _red, byte _green, byte _blue, byte _tailLength, int _startPosition, int _endPosition){
  red = _red;
  green = _green;
  blue = _blue;
  tailLength = _tailLength;
  startPosition = _startPosition;
  endPosition = _endPosition;
  actPosition = _startPosition;
  direction = true;

  positionHoldPeriod = _positionHoldPeriod; // only needed for moveTimeBased

  lastMoveTick = millis();
}

//Constructor for speed and gravitation based movement
Balls::Balls(int _speed, int _gravitation, byte _red, byte _green, byte _blue, byte _tailLength){
  red = _red;
  green = _green;
  blue = _blue;
  tailLength = _tailLength;
  direction = true;
  actPosition = 0;

  speed = _speed;                           // only needed for moveAgainstGravitation
  gravitation = _gravitation;               // only needed for moveAgainstGravitation
  startTime = millis();
}

Balls::Balls(){
   lastMoveTick = millis();
}

int Balls::moveTimeBased(){
  if (millis() - lastMoveTick >= positionHoldPeriod) {
    if (direction){
      actPosition++; 
    }else{
      actPosition--; 
    }
    if (actPosition == startPosition || actPosition == endPosition){
      direction = !direction;
    }
    lastMoveTick = millis();
  }
  return actPosition;
}

int Balls::moveAgainstGravitation(){
  //y = h + Vy * t – g * t² / 2
  //h = initial height -> can be ignored, always start at zero
  //Vy = speed 
  //t = millis passed since start
  //g = gravitation

  // init and reset
  if(actPosition <= 0){
    startTime = millis();
    actPosition = 0;
  }
  int timeSinceStart = millis() - startTime;
  actPosition = speed * (timeSinceStart/1000) - (gravitation * (timeSinceStart/1000) * (timeSinceStart/1000))/2;
  
  return actPosition;
}
