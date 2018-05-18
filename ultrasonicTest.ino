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
int FRONT = 345234;
int MAX_DIST_FROM_WALL = 10;

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

//If it makes two turns in the same direction, it must be inside a room. (NOPE)(wait yes)
//keep track of current direction it's facing. change it when you make a turn.
//start at 0, if make a right turn, add 1, left, subtract  one, if magnitude is 2, in room.

//If wall to front and right but not left, turn left.
//trapped in corner - back up until no wall to left or right
//

//@param side: LEFT or RIGHT (which wall to follow)
void followWall(int side){
  //The right sensor should always sense a wall
  //If the right sensor does not detect a wall, must turn right until see wall

  float sideSensor = (side == LEFT) ? leftSensor : rightSensor;

//TODO: For larger distances away from the wall, make it able to turn back onto course 
//      without hitting on the front
  

  bool wallLeft = isCloseWall(LEFT);
  bool wallRight = isCloseWall(RIGHT);
  bool wallFront = isCloseWall(FRONT);

  /** STAY WITH THE WALL **/
  forward(); //Go foward

  /** CASES **/
  if (wallLeft && wallFront && !wallRight){
    rotate(RIGHT);
  } else {
  if (sideSensor > MAX_DIST_FROM_WALL){  //If there is no wall to the right.      
    if (sideSensor > 20){  //If this is a sharp turn
      forward();  //move forward first to accommodate for the body size
      delay(500);
      turn(side); //Make a turn 
    } else {  //Otherwise, just maintain a close distance to the wall
    
      rotate(side);  //Rotate toward the wall
    }
    
  } else if (sideSensor < 1){
    rotate(oppositeDirection(side));
  } else {
    forward();
  }
  }

  

}

bool isCloseWall(int direction){
  return getSensorVal(direction) < MAX_DIST_FROM_WALL;
}

float getSensorVal(int direction){
  if (direction == LEFT){
    return leftSensor;
  }
  if (direction == RIGHT){
    return rightSensor;
  }
  if (direction == FRONT){
    return frontSensor;
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

int oppositeDirection(int direction){
  if (direction == LEFT){
    return RIGHT;
  }
  return LEFT;
}

