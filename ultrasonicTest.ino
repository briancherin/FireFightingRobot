//Ultrasonic sensors
int echoLeft = 8;
int trigLeft = 9;
int echoRight = 12;
int trigRight = 13;
int echoFront = 3;
int trigFront = 4;
int enabler = 2;

//HBridge motor control
int fwdPin = 5;           //Logic level output to the H-Bridge (Forward)
int revPin = 6;           //Another logic level output to the H-Bridge (Reverse)
int fwdPin2 = 10;
int revPin2 = 11;

//Sensor values
float frontSensor;
float rightSensor;
float leftSensor;

//constants
int LEFT = 13423;
int RIGHT = 43543;

bool goingForward = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(echoLeft, INPUT);
  pinMode(trigLeft, OUTPUT);
  pinMode(echoRight, INPUT);
  pinMode(trigRight, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(trigFront, OUTPUT);
  pinMode(enabler, OUTPUT);

  //Hbridge setup
  pinMode(fwdPin, OUTPUT); //Set the forward pin to an output
   pinMode(revPin, OUTPUT); //Set the forward pin to an output
   pinMode(fwdPin2, OUTPUT);
   pinMode(revPin2, OUTPUT);

  
  Serial.begin(9600);

  forward();
}

void loop() {

  frontSensor = getUltraSonicSensorVal(trigFront, echoFront);
  rightSensor = getUltraSonicSensorVal(trigRight, echoRight);
  leftSensor = getUltraSonicSensorVal(trigLeft, echoLeft);
 
  Serial.print("Left:");
  Serial.println(leftSensor); 
 // Serial.print("Right:");
 // Serial.println(rightSensor);
//  Serial.print("Front:");
//  Serial.println(frontSensor);
  

 followWall(LEFT);
 
}

//@param side: LEFT or RIGHT (which wall to follow)
void followWall(int side){
  //The right sensor should always sense a wall
  //If the right sensor does not detect a wall, must turn right until see wall

  float sideSensor = (side == LEFT) ? leftSensor : rightSensor;

  forward(); //Go foward
  if (sideSensor > 3){  //If there is no wall to the right
    if (sideSensor > 20){  //If this is a sharp turn
      forward();  //move forward first to accommodate for the body size
      delay(500);
      turn(side); //Make a turn 
    } else {  //Otherwise, just maintain a close distance to the wall
    
      rotate(side);  //Rotate toward the wall
    }
    
  } else {
    forward();
  }

}

float getUltraSonicSensorVal(int trig, int echo){

 //reset the  trigs
  digitalWrite(trig, LOW); 
  delayMicroseconds(2);

  //Send trigger for ten micro seconds
  digitalWrite(trig, HIGH);  
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  float distanceLeft =  pulseIn(echo, HIGH)  * 0.034 / 2;

  return distanceLeft;
}


void forward() {
  enableMotors();
  digitalWrite(fwdPin, HIGH); 
  digitalWrite(revPin, LOW);

  digitalWrite(fwdPin2, HIGH); 
  digitalWrite(revPin2, LOW);

  goingForward = true;
}

void reverse(){
  enableMotors();
  digitalWrite(fwdPin, LOW); 
  digitalWrite(revPin, HIGH);

  digitalWrite(fwdPin2, LOW); 
  digitalWrite(revPin2, HIGH);

  goingForward = false;
}

void stopMotors(){
  digitalWrite(enabler, LOW);
}

void enableMotors(){
  digitalWrite(enabler, HIGH);
}

void rotateRight(){
  digitalWrite(fwdPin, HIGH);
  digitalWrite(revPin, LOW);

  digitalWrite(fwdPin2, LOW);
  digitalWrite(revPin2, HIGH);
}

void rotateLeft(){
  digitalWrite(fwdPin, LOW);
  digitalWrite(revPin, HIGH);

  digitalWrite(fwdPin2, HIGH);
  digitalWrite(revPin2, LOW);
}

//@param direction: LEFT or RIGHT
void rotate(int direction) {
  if (direction == LEFT){
    rotateLeft();
  } else {
    rotateRight();
  }
}

//@param direction: LEFT or RIGHT
void turn(int direction){
  rotate(direction);
  delay(500);
}

