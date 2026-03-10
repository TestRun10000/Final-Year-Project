// Pin definitions
#include<EEPROM.h>
#include<Servo.h>

const int microsPin = 8;
const int stepPinX = 9;
const int dirPinX = 10;
const int stepPinY = 11;
const int dirPinY = 12;
const int enablePin = 2;


float distPerStep=0.8;
float distPerStepY=0.8;
float distPerDegZ=0.263;
float heightPerCard=0.266;

float currentX=0.0;
float currentY=0.0;
float currentZ=47.34;

float maxY=427.5;
float maxX=427.5;
float maxZ=47.34;
float maxDeckHeight=13.85;

Servo centreArm; 


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
    currentX= direction==FORWARD? currentX+distPerStep:currentX-distPerStep;
}

void rotateMotorY(int direction) {
    digitalWrite(dirPinY, direction);
    for (int i = 0; i < 200; i++) {
        digitalWrite(stepPinY, HIGH);
        delayMicroseconds(50);
        digitalWrite(stepPinY, LOW);
        delayMicroseconds(50);
    }
    currentY= direction==FORWARD? currentY+distPerStepY:currentY-distPerStepY;
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
    currentX= directionX==FORWARD? currentX+distPerStep:currentX-distPerStep;
    currentY= directionY==FORWARD? currentY+distPerStepY:currentY-distPerStepY;
}


unsigned int distToStepsX(float dist){
    return abs(dist)/distPerStep;
}

unsigned int distToStepsY(float dist){
    return abs(dist)/distPerStepY;
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
    if(stepsMade%0){}
    stepsMade++;
  }
  savePos();
}

void moveToPos(float x,float y,float z){

  moveDistance(x-currentX,y-currentY);
  savePos();
  if(z>=0 &&z<=maxZ){
    z=maxZ-z; 
    centreArm.write((int)(z/distPerDegZ));
  }

}

void loadPos(){
  EEPROM.get(0,currentX);
  EEPROM.get(4,currentY);
}

void savePos(){
  EEPROM.put(0,currentX);
  EEPROM.put(4,currentY);
}

void setup() {
    pinMode(microsPin, OUTPUT);
    pinMode(stepPinX, OUTPUT);
    pinMode(dirPinX, OUTPUT);
    pinMode(stepPinY, OUTPUT);
    pinMode(dirPinY, OUTPUT);
    pinMode(enablePin, OUTPUT);
    centreArm.attach(3);
    centreArm.write(0);
    Serial.begin(9600);
    savePos();
    Serial.print("\nX: ");Serial.print(currentX);
    Serial.print(" , Y: ");Serial.println(currentY);
    // Enable the driver
    digitalWrite(microsPin, HIGH);
    digitalWrite(enablePin, LOW);

    loadPos();
    Serial.print("X: ");Serial.print(currentX);
    Serial.print(" , Y: ");Serial.print(currentY);
    Serial.print(" , Z: ");Serial.println(currentZ);
    Serial.println("Done"); 
  // put your setup code here, to run once:
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');  // Read until newline
    Serial.print("You entered: ");
    input = input.substring(1, input.length() - 1);
    int commaIndex = input.indexOf(','); 
    double x = input.substring(0, commaIndex).toDouble(); 
    String yS = input.substring(commaIndex + 1);
    commaIndex = yS.indexOf(','); 
    double y = yS.substring(0,commaIndex).toDouble();
    double z = yS.substring(commaIndex + 1).toDouble();
    Serial.print(x);
    Serial.print(" , ");
    Serial.print(y);
    Serial.print(" , ");
    Serial.print(z);
    Serial.print("\n");
    moveToPos(x,y,z);
  }
  // put your main code here, to run repeatedly:

}
