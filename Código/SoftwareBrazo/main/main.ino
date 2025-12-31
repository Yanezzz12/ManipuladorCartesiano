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

int freq = 1000;
void goToOrigin(char axis = 'a')
{
  enableMotors(true);

  // Z-Axis // TODO: Define correct direction
  digitalWrite(pinDirZ, LOW);
  while((digitalRead(pinSZ) != 0) && (axis == 'a' || axis == 'z'))
  {
    waitOnStop();
    generateStep(pinStepZ, freq);
  } 
  if(digitalRead(pinSZ) == 1) // TODO: Check if sensor is active here
  { zPosition = 0.0f; }

  // XY-Axis
  while((digitalRead(pinSX != 0)) || (digitalRead(pinSY != 0))) //Este bucle no terminará TODO!
  {
    waitOnStop();

    // X-Axis
    digitalWrite(pinDirX1, LOW);  digitalWrite(pinDirX2, HIGH);  
    if(digitalRead(pinSX != 0) && (axis == 'a' || axis == 'x'))
    { generateStep(pinStepX1, freq); generateStep(pinStepX2, freq); } 

    // Y-Axis
    digitalWrite(pinDirY, LOW);  
    if(digitalRead(pinSY != 0) && (axis == 'a' || axis == 'y'))
    { generateStep(pinStepY, freq); }
  }
  if(digitalRead(pinSX) == 1) // TODO: Check of sensor is active here
  { xPosition = 0.0f; }
  if(digitalRead(pinSY) == 1) // TODO: Check of sensor is active here
  { yPosition = 0.0f; }

  enableMotors(false);
}

int moveSystem(float x, float y, float z, int c)
{
  moveXYZ(x, y, z);
  //moveClaw(c);
}

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
  
  enableMotors(true);
  do 
  { 
    // If stop button is pressed loop breaks 
    if(digitalRead(pinStop) == 0){ digitalWrite(pinMotEN, LOW); return; } 
    // nDir = 1 siempre debe ser movimiento en sentido contrario del sensor
    // nInterf == Movimiento NO causa interferencia
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

bool detectBand(int scanCycles, int scanFrequency) //TODO
{
  // Variable definition
  bool direction = 1; // Movimiento en sentido opuesto al sensor
  int count = 0;
  // MOVER A ORIGEN EN Y
  GoToOrigin('y');
  // Activates motors and starts
  digitalWrite(pinMotEN, HIGH);
  digitalWrite(pinDirY, direction);
  do 
  {
    // If stop button is pressed loop breaks (break or return)
    if(digitalRead(pinStop) == 0){ digitalWrite(pinMotEN, LOW); break; }
    // Effector movement
    generateStep(pinStepY, scanFrequency);
    // Updates position
    if(direction == 1)
    { yPosition += Smy; }
    else
    { yPosition -= Smy; }
    // Oscilating behaviour 
    if(digitalRead(pinSY) == HIGH) // Sensor físico Y presionado    
    { direction = 1; count++; }
    if(yPosition > yLimit)  
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

//-----------------------------
//-------Debug Functions-------
//-----------------------------

void enableMotors(bool enable)
{ // TODO: Check if correct
  digitalWrite(pinMotEN, enable ? HIGH : LOW);
}

void generateStep(int pinStepMotor, int frequency)
{
  digitalWrite(pinStepMotor, HIGH);
  delayMicroseconds(frequency);
  digitalWrite(pinStepMotor, LOW); 
  delayMicroseconds(frequency);
}

void waitOnStop()
{ 
  if(digitalRead(pinStop) == 0)
  { 
    enableMotors(false); 
    while(digitalRead(pinStop) == 0) 
    { delay(1); }
    enableMotors(true);
  }
}

bool exitOnStop()
{ // TODO: Check if correct
  if(digitalRead(pinStop) == 0) 
  { enableMotors(false); return true; }
  return false;
}

int returnPosition() // TODO: Hacer que se envíe una cadena
{
  // Creo no se puede retornar varias cosas a la vez
  return xPosition, yPosition, zPosition;
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

void loop(){}
