//Libraries
//#include <ros.h>

//Variable declaration
int frequencyDelayTime = 1000;
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

void setup() 
{
  Serial.begin(9600); //Check this value!         **************

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
  frequencyDelayTime = 1000 / frequency;
}

//Do NOT move axis X with this function
void MoveMotor(int dirPin, int motorPin, int steps, char direction)
{
  if(direction == '+')
  {
    digitalWrite(dirPin, HIGH);
    for(int x = 0; x < steps; x++) 
    {
      digitalWrite(motorPin, HIGH); 
      delayMicroseconds(frequencyDelayTime);      //**********
      digitalWrite(motorPin, LOW); 
      delayMicroseconds(frequencyDelayTime); 
    }
    delay(500);
  }
  else if(direction == '-')
  {
    digitalWrite(dirPin, LOW);
    for(int x = 0; x < steps; x++) 
    {
      digitalWrite(motorPin, HIGH); 
      delayMicroseconds(frequencyDelayTime); 
      digitalWrite(motorPin, LOW); 
      delayMicroseconds(frequencyDelayTime); 
    }
    delay(500);
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
  int revolutionSteps = 200;  //Check this information  *************
  float radiusPiece = 0.002f; //In meters [m]           *************
  float offset = 0.0f;        //In meters [m]           *************

  float revolutionDistance = (3.14159f * radiusPiece * 2) + offset;
  int steps = (distance * revolutionSteps) / revolutionDistance;

  return steps;
}

void MoveAxisX(int dirPinA, int dirPinB, int pinMotorA, int pinMotorB, float distance)  //Set boundaries      **********
{
  int steps = TransformDistance(distance);

  //Change direction if necessary *************************
  digitalWrite(dirPinA, HIGH); 
  digitalWrite(dirPinB, LOW);

  for(int x = 0; x < steps; x++) 
  {
    digitalWrite(pinMotorA, HIGH);
    digitalWrite(pinMotorB, HIGH);
    delayMicroseconds(frequencyDelayTime); 
    digitalWrite(pinMotorA, LOW);
    digitalWrite(pinMotorB, LOW);
    delayMicroseconds(frequencyDelayTime); 
  }
  delay(500);
}

//Do NOT move axis X with this function
void MoveAxisYZ(int dirPin, int pinMotorA, float distance)                              //Set boundaries      **********
{
  int steps = TransformDistance(distance);

  
}

void currentPosition()    //How can we make this function
{
  Serial.println("In process!");
}

//Check function construction         ***********
String commandStr;
String command1;
String command2;
String command3;

String ReadCommands()
{ 
  while (Serial.available() == 0) {} 
  commandStr = Serial.readString();
  commandStr.trim();  

  command1 = "";
  command2 = "";
  command3 = "";

  int wordCount = 0;
  for(int i = 0; i < commandStr.length(); i++)
  {
    if(commandStr[i] == ',')      
      wordCount++;
    else if(wordCount == 0)
      command1 += commandStr[i];
    else if(wordCount == 1)
      command2 += commandStr[i];
    else if(wordCount == 2)
      command3 += commandStr[i];
  }
  return command1, command2, command3;  
}

void loop() 
{


}

//Links
//https://howtomechatronics.com/tutorials/arduino/how-to-control-stepper-motor-with-a4988-driver-and-arduino/

