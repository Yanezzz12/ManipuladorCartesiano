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
// Defined physic limits of every axis
static float xLimit = 18.0;
static float yLimit = 22.0;
static float zLimit = 30.0;
// Minimum distance per step
static float Smx = 0.1885;
static float Smy = 0.1885;
static float Smz = 0.04;
// Global variables
float xPosition = 0.0f;
float yPosition = 0.0f;
float zPosition = 0.0f;
// Setup function
void setup()
{
  Serial.begin(115200);
  // Motors
  pinMode(pinStepX1, OUTPUT);
  pinMode(pinDirX1, OUTPUT);
  pinMode(pinStepX2, OUTPUT);
  pinMode(pinDirX2, OUTPUT);
  pinMode(pinStepY, OUTPUT);
  pinMode(pinDirY, OUTPUT);
  pinMode(pinStepZ, OUTPUT);
  pinMode(pinDirZ, OUTPUT);
  pinMode(pinMotEN, OUTPUT);
  // Sensors
  pinMode(pinSX, INPUT);
  pinMode(pinSY, INPUT);
  pinMode(pinSZ, INPUT);
  pinMode(pinSInd, INPUT);
  pinMode(pinSDist, INPUT);
  // Additional pins
  pinMode(pinServo, OUTPUT);
  pinMode(pinStop, INPUT);
  // Dynamixel setup
  delay(1000);  // Time for Dynamixel to start on power-up
  //ax12a.begin(BaudRate, pinServo, &Serial);
  //ax12a.setEndless(ID, ON);
}

//-----------------------------
//----------Functions----------
//-----------------------------

/*TODO: 
  ROS nodes
  Capability for routines
  Goto coords and move claw     DONE?
  IDLE function
  code optimization (static variables)
  error correction (re scan)
  routine feedback, it has finished
  Define correctly the supposed position of the system
  Functions completed
  >> 
*/

//-----------------------------
//-------Debug Functions-------
//-----------------------------

void printError(int numError)
{
  Serial.print("Error "); Serial.println(numError);
}

void enableMotors(bool enable) // TODO: Check if correct
{ 
  digitalWrite(pinMotEN, enable ? HIGH : LOW);
}

void setAxisDirection(char axis, bool againstOrigin) //TODO: Check directions
{ 
  switch(axis)
  {
    case 'x': 
      digitalWrite(pinDirX1, againstOrigin);
      digitalWrite(pinDirX2, !againstOrigin);
      break;
    case 'y': 
      digitalWrite(pinDirY, againstOrigin);
      break;
    case 'z': 
      digitalWrite(pinDirZ, againstOrigin);
      break;
    default:
      Serial.println("Error 404");
      break;
  }
}

bool axisAtOrigin(char axis) //Check if correct
{
  switch (axis) 
  {
    case 'x': return !digitalRead(pinSX); break;
    case 'y': return !digitalRead(pinSY); break;
    case 'z': return !digitalRead(pinSZ); break;
    default: printError(404);             break;
  }
}

void generateStep(int pinStepMotor, int frequency)
{
  digitalWrite(pinStepMotor, HIGH);
  delayMicroseconds(frequency);
  digitalWrite(pinStepMotor, LOW); 
  delayMicroseconds(frequency);
}

void waitOnStop() // TODO: Check if correct
{ 
  if(digitalRead(pinStop) == 0)
  { 
    enableMotors(false); 
    while(digitalRead(pinStop) == 0) 
    { delay(1); }
    enableMotors(true);
  }
}

bool exitOnStop() // TODO: Check if correct
{ 
  if(digitalRead(pinStop) == 0) 
  { enableMotors(false); return true; }
  return false;
}

void updatePosition(char axis, bool againstOrigin)
{
  switch (axis) 
  {
    case 'x': xPosition += againstOrigin ? Smx : -Smx; break;
    case 'y': yPosition += againstOrigin ? Smy : -Smy; break;
    case 'z': zPosition += againstOrigin ? Smz : -Smz; break;
    default:  printError(403);                         break;
  }
}

int returnPosition() // TODO: Hacer que se envíe una cadena
{
  return 1;
}

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
void testClaw()
{
  // Moves claw periodically
  ax12a.ledStatus(ID, ON);
  ax12a.turn(ID, LEFT, 300); 
  delay(5000);
  ax12a.ledStatus(ID, OFF);
  ax12a.turn(ID, RIGHT, 500);
  delay(5000);
} */

//-----------------------------
//----------Functions----------
//-----------------------------

void goToOrigin(char axis = 'a', int originFrequency = 1000)
{
  bool xAxisSelected = axis == 'a' || axis == 'x';
  bool yAxisSelected = axis == 'a' || axis == 'y';
  bool zAxisSelected = axis == 'a' || axis == 'z';
  enableMotors(true);
  
  setAxisDirection('z', 0);
  while(!axisAtOrigin('z') && zAxisSelected)
  {
    waitOnStop();
    generateStep(pinStepZ, originFrequency);
    updatePosition('z', 0);
  } 
  if(axisAtOrigin('z')) { zPosition = 0.0f; }

  // XY-Axis
  while((!axisAtOrigin('x') && xAxisSelected) || (!axisAtOrigin('y') && yAxisSelected)) 
  {
    waitOnStop();

    setAxisDirection('x', 0);
    if(!axisAtOrigin('x') && xAxisSelected)
    { generateStep(pinStepX1, originFrequency); generateStep(pinStepX2, originFrequency); updatePosition('x', 0); } 

    setAxisDirection('y', 0);
    if(!axisAtOrigin('y') && yAxisSelected)
    { generateStep(pinStepY, originFrequency); updatePosition('y', 0); }
  }
  if(axisAtOrigin('x')) { xPosition = 0.0f; }
  if(axisAtOrigin('y')) { yPosition = 0.0f; }

  enableMotors(false);
}

int freq = 1000;
void moveXYZ(float Sx, float Sy, float Sz)
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
  // Distance to steps conversion
  int xSteps = abs(Sx) / Smx;
  int ySteps = abs(Sy) / Smy;
  int zSteps = abs(Sz) / Smz;
  // Defines direction
  if(Sx >= 0)
  { setAxisDirection('x', 1); xDir = 1;   } 
  else
  { setAxisDirection('x', 0);  xDir = 0;  }
  if(Sy >= 0)
  { setAxisDirection('y', 1); yDir = 1;   } 
  else
  { setAxisDirection('y', 0);  yDir = 0;  }
  if(Sz >= 0)
  { setAxisDirection('z', 1); zDir = 1;   } 
  else
  { setAxisDirection('z', 0);  zDir = 0;  }
  
  enableMotors(true);
  do 
  { 
    // If stop button is pressed loop breaks 
    if(digitalRead(pinStop) == 0){ digitalWrite(pinMotEN, LOW); return; } // Correct with function

    bool xHasInterference = !digitalRead(pinSX) || xDir;
    bool yHasInterference = !digitalRead(pinSY) || yDir;
    bool zHasInterference = !digitalRead(pinSZ) || zDir;
    // While goal not reached and sensor is not pressed and movement doesn't cause interference
    bool xCond = (xMov != xGoal) && (xMov < xLimit) && xHasInterference;      // HAY QUE CAMBIAR NOMBRES 
    bool yCond = (yMov != yGoal) && (yMov < yLimit) && yHasInterference;
    bool zCond = (zMov != zGoal) && (zMov < zLimit) && zHasInterference;
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
      generateStep(pinStepX1, freq);
      generateStep(pinStepX2, freq);
      xMov += Smx;
      if(xDir == 0)     //Revisar que la dirección es la correcta
      { xPosition += Smx; } 
      else 
      { xPosition -= Smx; }
    }
    if(yCond)
    {
      generateStep(pinStepY, freq);
      yMov += Smy;
      if(yDir == 0)     //Revisar que la dirección es la correcta
      { yPosition += Smy; }
      else 
      { yPosition -= Smy; }
    }
    if(zCond)
    {
      generateStep(pinStepZ, freq);
      zMov += Smz;
      if(zDir == 0)     //Revisar que la dirección es la correcta
      { zPosition += Smz; }
      else 
      { zPosition -= Smz; }
    }
  }
  while(xCond || yCond || zCond);
  
  enableMotors(false);
}

/*
void moveClaw(int action)
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

int moveSystem(float x, float y, float z, int c)
{
  moveXYZ(x, y, z);
  //moveClaw(c);
}

bool detectBand(int scanCycles, int scanFrequency) //TODO
{
  // Variable definition
  bool direction = 1; // Movimiento en sentido opuesto al sensor
  int count = 0;
  // MOVER A ORIGEN EN Y
  goToOrigin('y');
  // Activates motors and starts
  enableMotors(true);
  setAxisDirection('y', 1);
  do 
  {
    // If stop button is pressed loop breaks (CHANGETO return)
    if(digitalRead(pinStop) == 0){ digitalWrite(pinMotEN, LOW); break; }
    // Effector movement
    generateStep(pinStepY, scanFrequency);
    // Updates position
    if(direction == 1)
    { yPosition += Smy; }
    else
    { yPosition -= Smy; }
    // Oscilating behaviour 
    if(axisAtOrigin('y')) // Sensor físico Y presionado    
    { direction = 1; count++; }
    if(yPosition > yLimit)  //Hacer consigna virtual
    { direction = 0; count++; }
  } 
  while(count < scanCycles);
  // Returns false if doesn't find band
  if(count > scanCycles) //Check if this works
  { return false; }
  // Centrar el brazo a la banda en el eje y TODO
  moveXYZ(0, 1, 0);
  return true;

  //TODO:
  /*
    Declarar limite en y
    Función para detectar banda
    Si no se detecta, repetir 2 veces más, (3 búsquedas en total)
    Si nunca se detecta entonces el proceso no continúa
    Si se detecta la banda, calcular centro y posicionarse
  */
}

void detectCap(float xPos) // TODO: Finish function
{
  // Static variables
  static int scanFreq = 1000;

  // If stop button is pressed loop breaks (break or return)
  // if(digitalRead(pinStop) == 0){ digitalWrite(pinMotEN, LOW); break; }



  /*TODO:
    Poner función de paro
    Mover en dirección opuesta al sensor
    Declarar limite en y
    Función para detectar banda
    Si no se detecta, repetir 2 veces más, (3 búsquedas en total)
    Si nunca se detecta entonces el proceso no continúa
    Si se detecta la tapa, calcular centro y posicionarse
  */
  return;
}

void loop(){}
