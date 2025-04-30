//https://www.youtube.com/watch?v=aYE_O_ZF2ac
#include <AX12A.h>

#define DirectionPin (35u)
#define BaudRate (1000000ul)
#define ID (1u)

void setup() {
  delay(1000);  // Give time for Dynamixel to start on power-up
  ax12a.begin(BaudRate, DirectionPin, &Serial);
  ax12a.setEndless(ID, ON);
}

void loop() {
  ax12a.ledStatus(ID, ON);
  ax12a.turn(ID, LEFT, 300);
  delay(5000);

  ax12a.ledStatus(ID, OFF);
  ax12a.turn(ID, RIGHT, 500);
  delay(5000);
}

/*
Hardware utilizado:
>> Dynamixel AX12A
>> ESP32 (ESP-WROOM-32)

El motor se alimenta a 12V y se conecta al pin TX de la garra
*/