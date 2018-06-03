//Ultrasonic sensors
int echoLeft = 31;
int trigLeft = 30;
int echoRight = 52;
int trigRight = 53;
int echoFront = 40;
int trigFront = 41;


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
int MAX_DIST_FROM_WALL = 20;
int MIN_DIST_FROM_WALL = 7;
int CLOSE_DIST_FROM_WALL = 10;
int SHARP_TURN_DIST = 80;
int NINETY_DEGREE_DELAY = 300;

int wallVariable = -1;
bool wallBool = false;

bool goingForward = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(echoLeft, INPUT);
  pinMode(trigLeft, OUTPUT);
  pinMode(echoRight, INPUT);
  pinMode(trigRight, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(trigFront, OUTPUT);
 // pinMode(enabler, OUTPUT);

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
 
//Serial.print("Left:");
//  Serial.println(leftSensor); 
 // Serial.print("Right:");
  //Serial.println(rightSensor);
  Serial.print("Front:");
  Serial.println(frontSensor);
  //
   //   Serial.println("JJJKKJLLK");

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
  if(frontSensor < 4 /*|| frontSensor > 1000*/){  //If the front sensor is too close to the wall, turn right.
    turn(oppositeDirection(side));
    Serial.println("front 2 close");
  }
  else if ((wallLeft && wallFront && !wallRight) || (wallRight && wallFront && !wallLeft)){
    Serial.print("Inside corner case\n");
    turnCloseToWall(side);   //Follow and turn through the corner
   
   Serial.print("frontSensor: ");
   Serial.println(frontSensor);
  } else {
    forward();
  if (sideSensor > 7){  //If we are far away from the wall     
    if (sideSensor > SHARP_TURN_DIST){  //If this is a sharp turn
      Serial.println(leftSensor);
      Serial.println("Sharp turn case: leftSensor= ");
      microTurn(side); //Make a turn 
    } else if(sideSensor < 15) {  //Kind of close to wall
      Serial.print("Maintain\n");
      //just maintain a close distance to the wall by drifting forward
      forward();
      delay(100);
    }
    else{ //Very far from wall
      turn(side);
  /*    int trackFrontSensor1 = frontSensor;
      forward();
      delay(50);
      updateSensorVals();
      int trackFrontSensor2 = frontSensor;
      Serial.print("From ");
      Serial.print(trackFrontSensor1);
      Serial.print(" to ");
      Serial.println(trackFrontSensor2);
      if(trackFrontSensor1 - trackFrontSensor2 > 0){
        Serial.println("Got closer to wall. Enter loop.");*/
        while(frontSensor > 15){
          updateSensorVals();
          forward();
          Serial.print("Forward to wall case ");
          Serial.println(frontSensor);

          if(getSensorVal(side) < 15){  //If stuck in loop but now close to wall
            break;                      //Get out of the loop and continue following
          }
          
        } 
    //  }
      
    }
  
    
  } else if (sideSensor < MIN_DIST_FROM_WALL || sideSensor > 1000){ //Too close to wall
    

    rotate(oppositeDirection);
    delay(50);
    forward();
    delay(100);
    
    Serial.println("hel");
  } else {
    forward();
    Serial.println("no cases, go forward");
  }
  }

  

}

bool isCloseWall(int direction){
  return getSensorVal(direction) < CLOSE_DIST_FROM_WALL;
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
  //enableMotors()
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
//  digitalWrite(enabler, LOW);
}

void enableMotors(){
//  digitalWrite(enabler, HIGH);
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

//@param direction of wall following: LEFT or RIGHT
void turnCloseToWall(int direction){
  //Serial.print(frontSensor);
  rotate(oppositeDirection(direction));
  delay(100);
  Serial.println("HIOERHOIHRFOEHIIORHF");
  while(!(leftSensor < 15)){ //Until it sees the wall again, make micro turns
    Serial.println(leftSensor);
    updateSensorVals();
    Serial.println("forward until see side wall");
    forward();
     delay(400);
    rotate(oppositeDirection(direction));
    delay(300);
    
  }
  Serial.println("Finished close wall turn");
}

//@param direction to turn: LEFT or RIGHT 
void microTurn(int direction){
  rotate(direction);
  delay(50);
  forward();
  delay(50);
  Serial.println("Finished 90 degree turn");
}

//@param direction to turn: LEFT or RIGHT (90 degree turn)
void turn (int direction){
  rotate(direction);
  delay(NINETY_DEGREE_DELAY);
}

int oppositeDirection(int direction){
  if (direction == LEFT){
    return RIGHT;
  }
  return LEFT;
}

void updateSensorVals(){
  frontSensor = getUltraSonicSensorVal(trigFront, echoFront);
  rightSensor = getUltraSonicSensorVal(trigRight, echoRight);
  leftSensor = getUltraSonicSensorVal(trigLeft, echoLeft);
}

