//Libraries
//#include <ros.h>

//Variable declaration
int delayTime = 1000;
const int frequency = 500; //Value in [Hz]

//Axis X, motor 1
const int stepX1;
const int dirX1;

//Axis X, motor 2
const int stepX2;
const int dirX2;

//Axis Y motor
const int stepY1;
const int dirY1;

//Axis Z motor
const int stepZ1;
const int dirZ1;
//(Creo que el motor sólo tiene dos entradas)

//Contact sensors
const int CSX;
const int CSY;
const int CSZ;

//Max torque step (Positive direction)


void setup() 
{
  Serial.begin(9600); //Check this value!

  //Motor pins
  pinMode(stepX1, OUTPUT);
  pinMode(dirX1, OUTPUT);
  pinMode(stepX2, OUTPUT);
  pinMode(dirX2, OUTPUT);
  pinMode(stepY1, OUTPUT);
  pinMode(dirY1, OUTPUT);
  pinMode(stepZ1, OUTPUT);
  pinMode(dirZ1, OUTPUT);
  
  //Sensor pins
  pinMode(CSX, INPUT);
  pinMode(CSY, INPUT);
  pinMode(CSZ, INPUT);

  //Frequency delay time
  delayTime = 1000 / frequency;
}

void RotateMotor(int dirPin, int motorPin, int steps, char direction)
{
  if(direction == '+')
  {
    digitalWrite(dirPin, HIGH);
    for(int x = 0; x < steps; x++) 
    {
      digitalWrite(motorPin, HIGH); 
      delayMicroseconds(500); 
      digitalWrite(motorPin, LOW); 
      delayMicroseconds(500); 
    }
    delay(delayTime);
  }
  else if(direction == '-')
  {
    digitalWrite(dirPin, LOW);
    for(int x = 0; x < steps; x++) 
    {
      digitalWrite(motorPin, HIGH); 
      delayMicroseconds(500); 
      digitalWrite(motorPin, LOW); 
      delayMicroseconds(500); 
    }
    delay(delayTime);
  }
}

int ReadContactSensor(int sensorPin)
{
  int state = digitalRead(sensorPin);
  return state;
}

//Transforms distance in [m] to steps in the motor (IN: Distance, OUT: steps) 
int TransformDistance(float distance)
{
  int revolutionSteps = 200;  //Check this information
  float radiusPiece = 0.002f; //In meters [m]
  float offset = 0.0f;        //In meters [m]

  float revolutionDistance = (3.14159f * radiusPiece * 2) + offset;
  int steps = (distance * revolutionSteps) / revolutionDistance;

  return steps;
}

void MoveAxisX(float distance)
{
  int steps = TransformDistance(distance);

  //We cannot use the RotateMotorFunction, how can we modify it?




}

void loop() 
{





}

//Links
//https://howtomechatronics.com/tutorials/arduino/how-to-control-stepper-motor-with-a4988-driver-and-arduino/

