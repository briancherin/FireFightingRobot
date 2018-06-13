//Ultrasonic sensors
int echoLeft = 31;
int trigLeft = 30;
int echoRight = 52;
int trigRight = 53;
int echoFront = 40;
int trigFront = 41;
int flameSensor = A0;

int enabler1 = 35;
int enabler2 = 37;

int fanPin = 47;


//HBridge motor control
int fwdPin = 5;           //Logic level output to the H-Bridge (Forward)
int revPin = 6;           //Another logic level output to the H-Bridge (Reverse)
int fwdPin2 = 10;
int revPin2 = 11;

//Sensor values
float frontSensor;
float rightSensor;
float leftSensor;
int fireVal;
int startFireVal; //updates every x seconds

//constants
int LEFT = 13423;
int RIGHT = 43543;
int FRONT = 345234;
int MAX_DIST_FROM_WALL = 20;
int MIN_DIST_FROM_WALL = 7;
int CLOSE_DIST_FROM_WALL = 10;
int SHARP_TURN_DIST = 80;
int NINETY_DEGREE_DELAY = 300;

//const int FIRE_THRESHOLD = 10;
const int FIRE_THRESHOLD = 500;

int wallVariable = -1;
bool wallBool = false;

bool goingForward = false;

unsigned long startTimeMillis;
unsigned long currTimeMillis;


void setup() {
  // put your setup code here, to run once:
  pinMode(echoLeft, INPUT);
  pinMode(trigLeft, OUTPUT);
  pinMode(echoRight, INPUT);
  pinMode(trigRight, OUTPUT);
  pinMode(echoFront, INPUT);
  pinMode(trigFront, OUTPUT);
  pinMode(enabler1, OUTPUT);
  pinMode(enabler2, OUTPUT);
  pinMode(fanPin, OUTPUT);

  //Hbridge setup
  pinMode(fwdPin, OUTPUT); //Set the forward pin to an output
   pinMode(revPin, OUTPUT); //Set the forward pin to an output
   pinMode(fwdPin2, OUTPUT);
   pinMode(revPin2, OUTPUT);

   digitalWrite(enabler1, HIGH);
   digitalWrite(enabler2, HIGH);

   startTimeMillis = millis();
   startFireVal = analogRead(flameSensor);

  
  Serial.begin(9600);

  forward();
}

void loop() {

  frontSensor = getUltraSonicSensorVal(trigFront, echoFront);
  rightSensor = getUltraSonicSensorVal(trigRight, echoRight);
  leftSensor = getUltraSonicSensorVal(trigLeft, echoLeft);
  fireVal = analogRead(flameSensor);
  Serial.print("Fire: ");
  Serial.println(fireVal);
 
Serial.print("Left:");
  Serial.println(leftSensor); 
 // Serial.print("Right:");
  //Serial.println(rightSensor);
 // Serial.print("Front:");
 // Serial.println(frontSensor);
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

  float sideSensor = (side == LEFT) ? leftSensor : rightSensor;

  bool wallLeft = isCloseWall(LEFT);
  bool wallRight = isCloseWall(RIGHT);
  bool wallFront = isCloseWall(FRONT);

  /** STAY WITH THE WALL **/
  forward(); //Go foward


  currTimeMillis = millis();
  if (currTimeMillis - startTimeMillis > 2000){
    
    startFireVal = fireVal;
    startTimeMillis = currTimeMillis;
    //Serial.println("Updating fire sensor");
  }

  
  
  if (startFireVal - fireVal >= FIRE_THRESHOLD){  //If there is a fire (meets threshold)
       Serial.print("THERES FIRE LOL: ");
          //Serial.println(startFireVal - fireVal);
          //rotate towards fire and moev forward for either a certain time or until a value is met 
          //goes for a certain amount until it sees something smaller, at smaller point rotate more until further smaller, else rotate back until original value
       int tempFireVal = fireVal; //this fireval was a "good value", use it as basis
       fireVal = analogRead(A0);
       
       while(fireVal - tempFireVal < 200){ //Rotate until the fireval increases by 10
        Serial.print("IN CLOSE FIRE LOOP: curr: ");
        Serial.print(fireVal);
        Serial.print(" --- tempFireVal: ");
        Serial.println(tempFireVal);
        
        rotate(RIGHT);
        fireVal = analogRead(A0);
        Serial.print("SOME STUPID FIFFERENE: ");
        Serial.println(fireVal - tempFireVal);
        
        if(fireVal < tempFireVal - 5){ 
          Serial.println("FOUND NEW MINIMUM");
          tempFireVal = fireVal; //setting new "center", need to rotate again
        }
       }

       while(abs(tempFireVal - fireVal) >= 50){  //rotate left until you're back at the min (plus or minus 50) (make the constant greater than you would think)
        Serial.print("NOPE GOING BACK: diff=");
        Serial.print(abs(tempFireVal - fireVal));
        Serial.print(", orig: ");
        Serial.print(tempFireVal);
        Serial.print(", curr: ");
        Serial.println(fireVal);
       rotate(LEFT);
       fireVal = analogRead(A0);
       }
       stopMotors();
       digitalWrite(fanPin, HIGH); //turns fan on after the motors are stopped - right in front of flame
       delay(3000);
       digitalWrite(fanPin, LOW);
       
       
       
  } else {
    Serial.print("diff: ");
    Serial.println(startFireVal-fireVal);
  }
    
  /** CASES **/
  if(frontSensor < 4 /*|| frontSensor > 1000*/){  //If the front sensor is too close to the wall, turn right.
    turn(oppositeDirection(side));
    Serial.println("front 2 close");
  }
else {
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
    

    rotate(oppositeDirection(side));
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
  digitalWrite(enabler1, LOW);
  digitalWrite(enabler2, LOW);
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

