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
#define pinStop     111
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
  //ax12a.begin(BaudRate, pinServo, &Serial);
  //ax12a.setEndless(ID, ON);
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

int freq = 1000;
void MoveXYZ(float Sx, float Sy, float Sz)
{
  // Variable declaration
  bool xDir = 0;
  bool yDir = 0;
  bool zDir = 0;
  bool xCond = 0;
  bool yCond = 0;
  bool zCond = 0;
  float xMov = 0.0;
  float yMov = 0.0;
  float zMov = 0.0;
  float xGoal = 1.0;
  float yGoal = 1.0;
  float zGoal = 1.0;
  // Defined physic limits of every axis
  static float xLimit = 18.0;
  static float yLimit = 22.0;
  static float zLimit = 30.0;
  // Minimum distance per step
  static float Smx = 0.1885;
  static float Smy = 0.1885;
  static float Smz = 0.04;
  // Distance to steps conversion
  int xSteps = abs(Sx) / Smx;
  int ySteps = abs(Sy) / Smy;
  int zSteps = abs(Sz) / Smz;
  // Defines direction
  if(Sx >= 0)
  { digitalWrite(pinDirX1, HIGH); digitalWrite(pinDirX2, HIGH); xDir = 1; } // Check xDir validity and check for compact solution
  else
  { digitalWrite(pinDirX1, LOW);  digitalWrite(pinDirX2, LOW);  xDir = 0; }
  if(Sy >= 0)
  { digitalWrite(pinDirY, HIGH); yDir = 1;  } // Check yDir validity
  else
  { digitalWrite(pinDirY, LOW);  yDir = 0;  }
  if(Sz >= 0)
  { digitalWrite(pinDirZ, HIGH); zDir = 1;  } // Chech zDir validity
  else
  { digitalWrite(pinDirZ, LOW);  zDir = 0;  }
  // Stepper motors activate
  digitalWrite(pinMotEN, HIGH);
  // Entering this loop allows movement
  do // Change to do while AND Do while conditions (3 conditions per variable?) (sensores y distancia)
  { 
    // If stop button is pressed loop breaks (break or return)
    if(digitalRead(pinStop) == 0){ digitalWrite(pinMotEN, LOW); break; } 
    // nDir = 1 siempre debe ser movimiento en sentido contrario del sensor
    // nInterf == Movimiento no causa interferencia
    bool xInterf = !digitalRead(pinSX) || xDir;
    bool yInterf = !digitalRead(pinSY) || yDir;
    bool zInterf = !digitalRead(pinSZ) || zDir;
    // While goal not reached and sensor is not pressed and movement doesn't cause interference
    bool xCond = (xMov != xGoal) && (xMov < xLimit) && xInterf; 
    bool yCond = (yMov != yGoal) && (yMov < yLimit) && yInterf;
    bool zCond = (zMov != zGoal) && (zMov < zLimit) && zInterf;
    /*
      Con Karnaugh obtenemos:
      S: Sensor
      D: Direccion
      M: Movimiento
      M = S' + D
      Asumiendo que 
      >> D = 0 es movimiento en dirección al sensor (cualquier eje)
      >> S = 0 es sensor NO presionado
    */
    xGoal = xSteps * Smx;
    yGoal = ySteps * Smy;
    zGoal = zSteps * Smz;
    // Effector movement
    if(xCond)
    {
      digitalWrite(pinStepX1, HIGH);
      digitalWrite(pinStepX2, HIGH);
      delayMicroseconds(freq);
      digitalWrite(pinStepX1, LOW);
      digitalWrite(pinStepX2, LOW);
      delayMicroseconds(freq);
      xMov += Smx;
    }
    if(yCond)
    {
      digitalWrite(pinStepY, HIGH);
      delayMicroseconds(freq);
      digitalWrite(pinStepY, LOW); 
      delayMicroseconds(freq);
      yMov += Smy;
    }
    if(zCond)
    {
      digitalWrite(pinStepZ, HIGH); 
      delayMicroseconds(freq);
      digitalWrite(pinStepZ, LOW); 
      delayMicroseconds(freq);
      zMov += Smz;
    }
  }
  while(xCond || yCond || zCond);
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
    digitalWrite(pinDirX1, LOW);  
    digitalWrite(pinDirX2, HIGH);  
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

/*
void MoveClaw(int action)
{
  static int open = 300;
  static int close = 500;

  if(action == 0)
  {
    ax12a.ledStatus(ID, ON);
    ax12a.turn(ID, LEFT, open); 
  }
  else if (action == 1)
  {
    ax12a.ledStatus(ID, OFF);
    ax12a.turn(ID, RIGHT, close);
  }
}*/

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

/*
void TestClaw()
{
  // Moves claw periodically
  ax12a.ledStatus(ID, ON);
  ax12a.turn(ID, LEFT, 300); 
  delay(5000);
  ax12a.ledStatus(ID, OFF);
  ax12a.turn(ID, RIGHT, 500);
  delay(5000);
} */

void loop()
{

}
