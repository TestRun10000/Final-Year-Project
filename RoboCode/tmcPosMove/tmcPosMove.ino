// Pin definitions
const int enablePin = 8;
const int stepPin = 9;
const int dirPin = 10;
const int ms1=11;
const int ms2=12;
int count=10;
float distPerStep=0.37;

#define FORWARD HIGH
#define BACKWARD LOW


void rotateMotor(int steps, int direction, int speed) {
    digitalWrite(dirPin, direction);
    for (int i = 0; i < steps; i++) {
        // Serial.println(i);
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(speed);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(speed);
    }
}


unsigned int distToSteps(float dist){
    return abs(dist)/distPerStep;
}

void moveDistance(float dist){
  int direction = dist>0 ? FORWARD:BACKWARD;
  Serial.println(direction);
  Serial.println(dist);
  int steps=distToSteps(dist);
  for(int i=0;i<steps;i++){
    rotateMotor(200,direction,50);
  }
  Serial.println("Done");
}


void setup() {
    pinMode(enablePin, OUTPUT);
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(ms1, OUTPUT);
    pinMode(ms2, OUTPUT);
    
    // Enable the driver
    digitalWrite(enablePin, LOW);
    digitalWrite(ms1, HIGH);
    digitalWrite(ms2, HIGH);
    
    Serial.begin(9600);
    Serial.println("TMC2209 Basic Control Ready");
    Serial.println(distToSteps(30.0));
    moveDistance(30.0);
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:

}
