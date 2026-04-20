// Pin definitions
#include<EEPROM.h>
#include<Servo.h>

const int microsPin = 8;
const int stepPinX = 9;
const int dirPinX = 10;
const int stepPinY = 11;
const int dirPinY = 12;
const int enablePin = 2;


float distPerStep=0.4;
float distPerStepY=1.65;
float distPerDegZ=0.263;
float heightPerCard=0.266;

float currentX=0.0;
float currentY=0.0;
float currentZ=47.34;

float maxY=427.5;
float maxX=427.5;
float maxZ=47.34;
float maxDeckHeight=13.85;

float gridStartX=0.0;
float gridStartY=0.0;


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

void penUp(){
  centreArm.write(20);
}
void penDown(){
  centreArm.write(114);
}

void drawC(){
  float radius = 36.0;
  float centerX = currentX + radius;
  float centerY = currentY + 80;
  
  penUp();
  delay(200);
  float targetX = centerX + 36 * sin(1.6 * PI);
  float targetY = centerY + 80 * cos(1.6 * PI);
  moveToPos(targetX, targetY, 17.095);
  // moveToPos(centerX - radius, centerY + 15, 0); 
  delay(200);
  penDown();
  delay(200);

  for (float angle = 1.6 * PI; angle > 0.4 * PI; angle -= 0.1) {
    float targetX = centerX + 36 * sin(angle);
    float targetY = centerY + 80 * cos(angle);
    moveToPos(targetX, targetY, 17.095);
    delay(50);
  }
  delay(200);
  penUp();
}

void drawE(){
  penDown();
  delay(200);
  penDown();
  delay(200);
  moveDistance(0,85);
  delay(200);
  penUp();
  delay(100);
  moveDistance(-2,-84);
  delay(100);
  penDown();
  delay(100);
  moveDistance(45,0);
  moveDistance(0,80);
  delay(100);
  penUp();
  delay(100);
  moveDistance(-14,-80);
  penDown();
  delay(100);
  moveDistance(45,0);
  delay(100);
  moveDistance(0,80);
  delay(100);
  penUp();
}

void drawS(){
    float radius = 40.0;
  float centerX = currentX + 20;
  float centerY = currentY + 40;
  
  penUp();
  delay(200);
  float targetX = centerX + 20 * sin(0 * PI);
  float targetY = centerY + 40 * cos(0 * PI);
  moveToPos(targetX, targetY, 17.095);
  // moveToPos(centerX - radius, centerY + 15, 0); 
  delay(200);
  penDown();
  delay(200);

  for (float angle = 0 * PI; angle < 1.5 * PI; angle += 0.1) {
    float targetX = centerX + 20 * sin(angle);
    float targetY = centerY + 40 * cos(angle);
    moveToPos(targetX, targetY, 17.095);
    delay(50);
  }
  centerX = currentX - 20;
  // centerY = currentY + 40;
  for (float angle = 0.5 * PI; angle > 0; angle -= 0.1) {
    float targetX = centerX + 20 * sin(angle);
    float targetY = centerY + 40 * cos(angle);
    moveToPos(targetX, targetY, 17.095);
    delay(50);
  }
  for (float angle = 2*PI; angle > 1*PI; angle -= 0.1) {
    float targetX = centerX + 20 * sin(angle);
    float targetY = centerY + 40 * cos(angle);
    moveToPos(targetX, targetY, 17.095);
    delay(50);
  }
  delay(200);
  penUp();
}

void drawX(){
  delay(200);
  penDown();
  delay(200);
  for(int i=0;i<82;i++){
      moveDistance(1,2);
  }
  delay(100);
  penUp();
  delay(100);
  moveDistance(0,-130);
  delay(100);
  penDown();
  delay(100);
  for(int i=0;i<82;i++){
      moveDistance(-1,2);
  }
  delay(100);
  penUp();
}

void writeEssex(){
  float startX=currentX;
  float startY=currentX;
  penUp();
  delay(100);
  drawE(); 
  delay(100);
  penUp(); 
  moveToPos(startX+35,startY+25,0);
  delay(100);
  drawS(); 
  delay(100);
  penUp();
  delay(200);
  moveToPos(startX+35,startY+115,0);
  delay(100);
  drawS(); delay(100);
  moveToPos(startX,startY+220,0);
  penUp();
  delay(100);
  drawE();
  penUp();
  delay(100);
  moveToPos(startX,startY+310,0);
  penUp();
  delay(100);
  drawX();
  penUp();
  delay(100);
}

void writeCSEE(){
  // moveToPos(160,100,0);
  float startX=currentX;
  float startY=currentX;
  penUp();
  delay(100);
  drawC(); 
  delay(100);
  penUp(); 
  moveToPos(startX+35,startY+118,0);
  delay(100);
  drawS(); 
  delay(100);
  penUp();
  delay(200);
  moveToPos(startX,startY+220,0);
  delay(100);
  drawE(); delay(100);
  moveToPos(startX,startY+320,0);
  penUp();
  delay(100);
  drawE();
  penUp();
  delay(100);
}

void drawCross(){
  delay(200);
  penDown();
  delay(200);
  for(int i=0;i<41;i++){
      moveDistance(1,2);
  }
  delay(100);
  penUp();
  delay(100);
  moveDistance(0,-70);
  delay(100);
  penDown();
  delay(100);
  for(int i=0;i<41;i++){
      moveDistance(-1,2);
  }
  delay(100);
  penUp();

}

void drawNaught(){
  float centerX = currentX + 20;
  float centerY = currentY + 40;
  moveToPos(centerX, centerX, 47.34);
  // penDown();
  // delay(200);
  for (float angle = 0; angle <= 2*PI; angle += 0.1) {
    float targetX = centerX + 20 * sin(angle);
    float targetY = centerY + 40 * cos(angle);
    moveToPos(targetX, targetY, 17.095);
    delay(50);
  }
  
  for (float angle = 0; angle <= 0.5; angle += 0.1) {
    float targetX = centerX + 20 * sin(angle);
    float targetY = centerY + 40 * cos(angle);
    moveToPos(targetX, targetY, 17.095);
    delay(50);
  }
  
  penUp();
  delay(100);
  moveToPos(centerX, centerX, 47.34);

}

void placePoint(int x,int y,int player){
  if(player==1){
    Serial.print(gridStartX+x*45);Serial.print(",");Serial.println(gridStartY+y*90);
    moveToPos(gridStartX+x*45,gridStartY+y*90,47.54);
    delay(200);
    drawNaught();
  }
  else if(player==2){

  }
  Serial.println("done");
}

void drawGrid(){
  penDown();
  delay(200);
  moveDistance(0,300);
  delay(200);
  penUp();
  delay(100);
  moveDistance(45,20);
  delay(100);
  penDown();
  delay(100);
  moveDistance(0,-300);
  delay(100);
  penUp();
  delay(100);
  moveDistance(-90,100);
  delay(100);
  penDown();
  delay(100);
  moveDistance(130,0);
  delay(100);
  penUp();
  delay(100);
  moveDistance(0,90);
  delay(100);
  penDown();
  delay(100);
  moveDistance(-130,0);
  delay(100);
  penUp();
  delay(100);
  moveDistance(70,-50);
  delay(100);
  gridStartX=currentX+15;
  gridStartY=currentY-130;
  Serial.print(gridStartX);Serial.print(",");Serial.println(gridStartY);
}

void gameLoop(){
  drawGrid();
  player=1;
  while(true){
    if(Serial.available()){
      String input = Serial.readStringUntil('\n');
      if(input=="done") return;
      else{
        input = input.substring(1, input.length() - 1);
        int commaIndex = input.indexOf(','); 
        double x = input.substring(0, commaIndex).toDouble(); 
        String yS = input.substring(commaIndex + 1);
        commaIndex = yS.indexOf(','); 
        double y = yS.substring(0,commaIndex).toDouble();
        placePoint(x,y,player);
        player=player==1? 2:1;
      }
    }
  }

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
    
 

  // put your setup code here, to run once:
}



void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');  // Read until newline
    Serial.print("You entered: ");
    if(input=="home"){
      moveToPos(0,0,47);
      Serial.println("done");
    }
    else if(input=="pos"){
      String input = Serial.readStringUntil('\n');  // Read until newline
      input = input.substring(1, input.length() - 1);
      int commaIndex = input.indexOf(','); 
      double x = input.substring(0, commaIndex).toDouble(); 
      String yS = input.substring(commaIndex + 1);
      commaIndex = yS.indexOf(','); 
      double y = yS.substring(0,commaIndex).toDouble();
      double z = yS.substring(commaIndex + 1).toDouble();
      moveToPos(x,y,z);
      Serial.println("done");
    }
    else if(input=="write"){
      moveToPos(160,150,47.34);
      writeEssex(); 
      moveToPos(60,80,47.34);
      writeCSEE();
      Serial.println("done");
      
    }
    else if(input=="game"){
      gameLoop();
    }      
  }
  // put your main code here, to run repeatedly:

}
