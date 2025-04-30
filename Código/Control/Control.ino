//Pins
const int pinDirX1 = 33;
const int pinStepX1 = 32; 
const int pinEN = 25;
//Valores
const int stepsPerRev = 200;

void setup() 
{
  Serial.begin(115200);
  pinMode(pinDirX1, OUTPUT);
  pinMode(pinStepX1, OUTPUT);
  pinMode(pinEN, OUTPUT);
}

void MoveMotor()
{
  digitalWrite(pinEN, 0);
  digitalWrite(pinDirX1, HIGH);

  for(int i = 0; i < stepsPerRev; i++)
  {
    digitalWrite(pinStepX1, HIGH);
    //delay(1000);
    digitalWrite(pinStepX1, LOW);
    //delay(1500);
    Serial.print(i);
  }
  digitalWrite(pinEN, 1);
}

void loop() 
{ 
  MoveMotor();
  delay(2000);
}
