// Pin definitions
const int microsPin = 8;
const int stepPinX = 9;
const int dirPinX = 10;
const int stepPinY = 11;
const int dirPinY = 12;
const int enablePin = 2;


int count=10;
float distPerStep=0.76;
float distPerStepY=0.37;
// 651
#define FORWARD HIGH
#define BACKWARD LOW


void rotateMotorX(int direction) {
    digitalWrite(dirPinX, direction);
    for (int i = 0; i < 200; i++) {
        // Serial.println(i);
        digitalWrite(stepPinX, HIGH);
        delayMicroseconds(50);
        digitalWrite(stepPinX, LOW);
        delayMicroseconds(50);
    }
}

void rotateMotorY(int direction) {
    digitalWrite(dirPinX, direction);
    for (int i = 0; i < 200; i++) {
        digitalWrite(stepPinY, HIGH);
        delayMicroseconds(50);
        digitalWrite(stepPinY, LOW);
        delayMicroseconds(50);
    }
}

void rotateMotors(int directionX,int directionY) {
    digitalWrite(dirPinY, directionY);
    digitalWrite(dirPinX, directionX);
    for (int i = 0; i < 200; i++) {
        digitalWrite(stepPinY, HIGH);
        digitalWrite(stepPinX, HIGH);
        delayMicroseconds(50);
        digitalWrite(stepPinY, LOW);
        digitalWrite(stepPinX, LOW);
        delayMicroseconds(50);
    }
}


unsigned int distToStepsX(float dist){
    return abs(dist)/distPerStep;
}

unsigned int distToStepsY(float dist){
    return abs(dist)/distPerStepY;
}

void moveDistanceX(float dist){
  int direction = dist>0 ? FORWARD:BACKWARD;
  Serial.println(direction);
  Serial.println(dist);
  int steps=distToStepsX(dist);
  for(int i=0;i<steps;i++){
    rotateMotorX(direction);
  }
  Serial.println("Done");
}

void moveDistance(float x,float y){
  int directionX=x>0?FORWARD:BACKWARD;
  int directionY=y>0?FORWARD:BACKWARD;
  int xSteps=distToStepsX(x);
  int ySteps=distToStepsY(y);
  int stepsMade=0;
  while(stepsMade<xSteps || stepsMade<ySteps){
    if(stepsMade<xSteps && stepsMade<ySteps){
      rotateMotors(directionX,directionY);
    }
    else if(stepsMade<xSteps){
        rotateMotorX(directionX);
    }
    else if(stepsMade<ySteps){
      rotateMotorY(directionY);
    }
    stepsMade++;
  }
}


void setup() {
    pinMode(microsPin, OUTPUT);
    pinMode(stepPinX, OUTPUT);
    pinMode(dirPinX, OUTPUT);
    pinMode(stepPinY, OUTPUT);
    pinMode(dirPinY, OUTPUT);
    pinMode(enablePin, OUTPUT);
    
    // Enable the driver
    digitalWrite(microsPin, HIGH);
    digitalWrite(enablePin, LOW);
    
    Serial.begin(9600);
    Serial.println("TMC2209 Basic Control Ready");
    // moveDistance(60.0);
    moveDistance(150,-150);
    Serial.println("Done"); 
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:

}
