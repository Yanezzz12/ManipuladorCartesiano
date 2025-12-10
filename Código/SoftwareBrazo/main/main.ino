//-----------------------------
//----------Libraries----------
//-----------------------------

//#include "include/functions.c"
//#include <AX12A.h>

//-----------------------------
//------------Setup------------
//-----------------------------

// Pines Motores a Pasos
#define pinStepX1   32 
#define pinDirX1    33 
#define pinStepX2   25
#define pinDirX2    26
#define pinStepY    19 
#define pinDirY     21
#define pinStepZ    22
#define pinDirZ     23
#define pinMotEN    27
// Pines sensores
#define pinSX       100
#define pinSY       101
#define pinSZ       102
#define pinSInd     103
#define pinSDist    104
// Pines adicionales
#define pinParo     111
// Pines servomotor
#define pinServo    (35u)
#define BaudRate    (1000000ul)
#define ID          (1u)

void setup()
{
  Serial.begin(115200);
  // Declaración de motores
  pinMode(pinStepX1, OUTPUT);
  pinMode(pinDirX1, OUTPUT);
  pinMode(pinStepX2, OUTPUT);
  pinMode(pinDirX2, OUTPUT);
  pinMode(pinStepY, OUTPUT);
  pinMode(pinDirY, OUTPUT);
  pinMode(pinStepZ, OUTPUT);
  pinMode(pinDirZ, OUTPUT);
  pinMode(pinMotEN, OUTPUT);
  // Declaración de sensores
  pinMode(pinSX, INPUT);
  pinMode(pinSY, INPUT);
  pinMode(pinSZ, INPUT);
  pinMode(pinSInd, INPUT);
  pinMode(pinSDist, INPUT);
  // Declaraciones adicionales
  pinMode(pinServo, OUTPUT);
  pinMode(pinStop, INPUT);
  //Dynamixel setup
  delay(1000);  // Time for Dynamixel to start on power-up
  ax12a.begin(BaudRate, pinServo, &Serial);
  ax12a.setEndless(ID, ON);
}

//-----------------------------
//----------Functions----------
//-----------------------------


int positionControl(float x, float y, float z)
{
  /*
  MoveToOrigin();
  MoveXYZ();
  */
}

//TODO: We are not considering stop button and sensors, we have to.
int freq = 1000;
void MoveXYZ(float Sx, float Sy, float Sz)
{
  // Variable declaration
  float xPos = 0;
  float yPos = 0;
  float zPos = 0;
  // Minimum distance per step
  float Smx = 0.1885;
  float Smy = 0.1885;
  float Smz = 0.04;
  // Distance to steps conversion
  // TODO: Check conversion
  int xSteps = abs(Sx) / Smx;
  int ySteps = abs(Sy) / Smy;
  int zSteps = abs(Sz) / Smz;
  // Defines direction
  if(Sx >= 0)
  { digitalWrite(pinDirX1, HIGH); digitalWrite(pinDirX2, HIGH); }
  else
  { digitalWrite(pinDirX1, LOW);  digitalWrite(pinDirX2, LOW);  }
  if(Sy >= 0)
  { digitalWrite(pinDirY, HIGH); }
  else
  { digitalWrite(pinDirY, LOW);  }
  if(Sz >= 0)
  { digitalWrite(pinDirZ, HIGH); }
  else
  { digitalWrite(pinDirZ, LOW);  }
  // Stepper motors activate
  digitalWrite(pinMotEN, HIGH);
  while((xPos != xSteps * Smx) && (yPos != ySteps * Smy) && (zPos != zSteps * Smz)) // TODO: Check if better condition (I think it has to be OR, not AND)
  {
    // Goals (Do I leave it like this? I have to declare it)
    xGoal = xSteps * Smx;
    yGoal = ySteps * Smy;
    zGoal = zSteps * Smz;
    // If stop button is pressed loop breaks
    if(digitalRead(pinStop) == 0)
    { break; }

    if(xPos != xSteps * Smx) //OR axis distance has been surpassed
    {
      digitalWrite(pinStepX1, HIGH);
      digitalWrite(pinStepX2, HIGH);
      delayMicroseconds(freq);
      digitalWrite(pinStepX1, LOW);
      digitalWrite(pinStepX2, LOW);
      delayMicroseconds(freq);
      xPos += Smx;
    }
    if(yPos != ySteps * Smy) //OR axis distance has been surpassed
    {
      digitalWrite(pinStepY, HIGH);
      delayMicroseconds(freq);
      digitalWrite(pinStepY, LOW); 
      delayMicroseconds(freq);
      yPos += Smy;
    }
    if(zPos != zSteps * Smz) //OR axis distance has been surpassed
    {
      digitalWrite(pinStepZ, HIGH); 
      delayMicroseconds(freq);
      digitalWrite(pinStepZ, LOW); 
      delayMicroseconds(freq);
      zPos += Smz;
    }
  }
  // Stepper motors deactivate
  digitalWrite(pinMotEN, LOW);
}

void GoToOrigin() 
{
  // Enables motors
  digitalWrite(pinMotEN, HIGH);
  // Moves axis Z to origin
  digitalWrite(pinDirZ, LOW);
  while(digitalRead(pinSZ) != 0) 
  {
    // System pauses if pinParo is pressed
    while(digitalRead(pinStop) == 0) { }
    digitalWrite(pinStepZ, HIGH);
    delayMicroseconds(freq);
    digitalWrite(pinStepZ, LOW);
    delayMicroseconds(freq);
  }
  // Moves axis XY to origin
  while((digitalRead(pinSX != 0)) || (digitalRead(pinSY != 0))) 
  {
    // If stop button is pressed system pauses
    while(digitalRead(pinStop) == 0) { }
    // Simultaneous movement to origin
    digitalWrite(pinDirX, LOW);  
    if(digitalRead(pinSX != 0))
    {
      digitalWrite(pinStepX1, HIGH);
      digitalWrite(pinStepX2, HIGH);
      delayMicroseconds(freq);
      digitalWrite(pinStepX1, LOW);
      digitalWrite(pinStepX2, LOW);
      delayMicroseconds(freq);
    }
    digitalWrite(pinDirY, LOW);  
    if(digitalRead(pinSY != 0))
    {
      digitalWrite(pinStepY, HIGH);
      delayMicroseconds(1000);
      digitalWrite(pinStepY, LOW);
      delayMicroseconds(1000);
    }
  }
  digitalWrite(pinMotEN, LOW);
}

void MoveClaw(int action)
{
  if(action == 0)
  {
    ax12a.ledStatus(ID, ON);
    ax12a.turn(ID, LEFT, 300); 
  }
  else if (action == 1)
  {
    ax12a.ledStatus(ID, OFF);
    ax12a.turn(ID, RIGHT, 500);
  }
}

//-----------------------------
//-------Debug Functions-------
//-----------------------------

void showSensors()
{
  Serial.print("Sensor eje X: ");
  Serial.println(digitalRead(pinSX));
  Serial.print("Sensor eje Y: ");
  Serial.println(digitalRead(pinSY));
  Serial.print("Sensor eje Z: ");
  Serial.println(digitalRead(pinSZ));
  Serial.print("Sensor inductivo: ");
  Serial.println(digitalRead(pinSInd));
  Serial.print("Sensor distancia: "); 
  Serial.println(analogRead(pinSDist)); // Sensor analógico
}

void TestClaw()
{
  // Moves claw periodically
  ax12a.ledStatus(ID, ON);
  ax12a.turn(ID, LEFT, 300); 
  delay(5000);
  ax12a.ledStatus(ID, OFF);
  ax12a.turn(ID, RIGHT, 500);
  delay(5000);
}

void loop()
{

}
