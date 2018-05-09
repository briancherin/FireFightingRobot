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
  leftSensor = getUltraSonicSensorVal(trigFront, echoFront);
 
  Serial.print("Left:");
  Serial.println(leftSensor); 
  Serial.print("Right:");
  Serial.println(rightSensor);
  Serial.print("Front:");
  Serial.println(frontSensor);
  
/*
  if (frontSensor < 7) { //See wall
    if (goingForward) reverse(); //Reverse
  } else {  //No wall
    if (!goingForward) forward();  //Forward
  }
 */
 followRightWall();


}

void followRightWall(){
  //The right sensor should always sense a wall
  //If the right sensor does not detect a wall, must turn right until see wall

  forward(); //Go foward
  if (rightSensor > 7){  //If there is no wall to the right
    delay(1000);  //Let the body pass the turn (TODO: THIS BREAKS IT)
    turnRight();  //Rotate right
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

void turnRight(){
  digitalWrite(fwdPin, HIGH);
  digitalWrite(revPin, LOW);

  digitalWrite(fwdPin2, LOW);
  digitalWrite(revPin2, HIGH);
}

void turnLeft(){
  digitalWrite(fwdPin, LOW);
  digitalWrite(revPin, HIGH);

  digitalWrite(fwdPin2, HIGH);
  digitalWrite(revPin2, LOW);
}

