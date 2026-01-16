//-----------------------------
//----------Libraries----------
//-----------------------------

//#include "include/functions.c"
//#include <AX12A.h>

//-----------------------------
//------------Setup------------
//-----------------------------

// Pines Motores a Pasos
const int pinStepX1 = 32;
const int pinDirX1 =  33;
const int pinStepX2 = 25;
const int pinDirX2 =  26;
const int pinStepY =  19;
const int pinDirY =   21;
const int pinStepZ =  22;
const int pinDirZ =   23;
const int pinMotEN =  27;
// Pines sensores
const int pinSX =     100;
const int pinSY =     101;
const int pinSZ =     102;
const int pinSInd =   103;
const int pinSDist =  104;
// Pines adicionales
const int pinStop =   111;
// Pines servomotor
#define pinServo    (35u)
#define BaudRate    (1000000ul)
#define ID          (1u)
// Defined physic limits of every axis
const float xLimit = 18.0;
const float yLimit = 22.0;
const float zLimit = 30.0;
// Minimum distance per step
const float Smx = 0.1885;
const float Smy = 0.1885;
const float Smz = 0.04;
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

bool stopPressed()  //TODO: Check if correct
{
  return !digitalRead(pinStop);
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
  if(stopPressed())
  { 
    enableMotors(false); 
    while(digitalRead(pinStop) == 0) 
    { delay(1); }
    enableMotors(true);
  }
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


void moveXYZ(float Sx, float Sy, float Sz, int frequency = 1000) // Input in [mm]
{
  // Variable declaration
  bool xMotionAllowed = false;
  bool yMotionAllowed = false;
  bool zMotionAllowed = false;
  bool xAxisMovement = false;
  bool yAxisMovement = false;
  bool zAxisMovement = false;
  float xDisplacement = 0.0f;
  float yDisplacement = 0.0f;
  float zDisplacement = 0.0f;
  // Distance to steps conversion
  int xSteps = abs(Sx) / Smx;
  int ySteps = abs(Sy) / Smy;
  int zSteps = abs(Sz) / Smz;
  // Goal calculus
  float xGoal = xSteps * Smx;
  float yGoal = ySteps * Smy;
  float zGoal = zSteps * Smz;
  // Defines direction
  bool xMovingAgainstOrigin = Sx >= 0;
  bool yMovingAgainstOrigin = Sy >= 0;
  bool zMovingAgainstOrigin = Sz >= 0;
  // Setting direction
  setAxisDirection('x', xMovingAgainstOrigin);
  setAxisDirection('y', yMovingAgainstOrigin);
  setAxisDirection('z', zMovingAgainstOrigin);

  enableMotors(true);
  do 
  { 
    if(stopPressed()){ enableMotors(false); return; }

    // Obtained with Karnaugh (Check notes)
    xMotionAllowed = !axisAtOrigin('x') || xMovingAgainstOrigin;
    yMotionAllowed = !axisAtOrigin('y') || yMovingAgainstOrigin;
    zMotionAllowed = !axisAtOrigin('z') || zMovingAgainstOrigin;
    
    xAxisMovement = (xDisplacement != xGoal) && (xPosition < xLimit) && xMotionAllowed;
    yAxisMovement = (yDisplacement != yGoal) && (yPosition < yLimit) && yMotionAllowed;
    zAxisMovement = (zDisplacement != zGoal) && (zPosition < zLimit) && zMotionAllowed;
    
    if(xAxisMovement)
    {
      generateStep(pinStepX1, frequency);
      generateStep(pinStepX2, frequency); 
      updatePosition('x', xMovingAgainstOrigin);
      xDisplacement += Smx;
    }
    if(yAxisMovement)
    {
      generateStep(pinStepY, frequency);
      updatePosition('y', yMovingAgainstOrigin);
      yDisplacement += Smy;
    }
    if(zAxisMovement)
    {
      generateStep(pinStepZ, frequency);
      updatePosition('z', zMovingAgainstOrigin);
      zDisplacement += Smz;
    }
  }
  while(xAxisMovement || yAxisMovement || zAxisMovement);
  
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

int moveSystem(float x, float y, float z, int c, int frequency = 1000)
{
  moveXYZ(x, y, z, frequency);
  //moveClaw(c);
}

bool smartDirectionSwitch(char smartAxis, int maxSwitch)
{ 
  //TODO: Quitar count++ y ver cómo colocarlo mejor ()
  static bool axisOriginLocked = false;
  static bool smartDirection = false;
  int count = 0;

  if(axisAtOrigin(smartAxis) && !axisOriginLocked) { smartDirection = 1; count++; }
  if(!axisAtOrigin(smartAxis)) { axisOriginLocked = false; }
  switch(smartAxis) 
  {
    case 'x':
      if(xPosition > xLimit)  { smartDirection = 0; count++; }
      break;
    case 'y':
      if(yPosition > yLimit)  { smartDirection = 0; count++; }
      break;
    case 'z':
      if(zPosition > zLimit)  { smartDirection = 0; count++; }
      break;
  }
  setAxisDirection(smartAxis, smartDirection);





  // Oscilating behaviour 
  if(axisAtOrigin('y') && !yOriginLocked)   { direction = 1; count++; }
  if(yPosition > yLimit)  { direction = 0; count++; }
  if(!axisAtOrigin('y'))  { yOriginLocked = false; }
}


bool detectBand(int scanCycles, int scanFrequency) //TODO
{
  // Variable declaration
  static bool yOriginLocked = false;
  bool direction = 1; 
  int count = 0;

  goToOrigin('y');
  setAxisDirection('y', direction);
  enableMotors(true);

  do 
  {
    if(stopPressed()){ enableMotors(false); return false; }
    
    generateStep(pinStepY, scanFrequency);
    updatePosition('y', direction);
    smartDirectionSwitch('y', scanCycles); //Cambia dirección si llega a límites
  } 
  while(count < scanCycles || false); //Se ejecuta Si oscila y no encuentra nada o si encuentra algo (Plantear)
  
  // Returns false if doesn't find band
  if(count > scanCycles) // Check 
  { return false; }

  moveXYZ(0, 1, 0); //TODO: Calculate center
  return true;

  //TODO:
  /*
    Función para detectar bordes de banda
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
