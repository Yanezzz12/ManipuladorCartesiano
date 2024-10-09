void setup() 
{
  Serial.begin(9600);  // Configurar el puerto serial a 9600 baudios
  Serial.println("Arduino Mega listo para recibir comandos");
  pinMode(13, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');  // Leer la señal entrante
    Serial.flush();
    Serial.println("ACK");  // Enviar confirmación

    if (command.startsWith("X+")) {
      // Mover el eje X en dirección positiva
      moverEjeX(1);
    } else if (command.startsWith("X-")) {
      // Mover el eje X en dirección negativa
      moverEjeX(-1);
    } else if (command.startsWith("Y+")) {
      // Mover el eje Y en dirección positiva
      moverEjeY(1);
    } else if (command.startsWith("Y-")) {
      // Mover el eje Y en dirección negativa
      moverEjeY(-1);
    } else if (command.startsWith("Z+")) {
      // Mover el eje Z en dirección positiva
      moverEjeZ(1);
    } else if (command.startsWith("Z-")) {
      // Mover el eje Z en dirección negativa
      moverEjeZ(-1);
    } else if (command.startsWith("GOPEN")) {
      // Abrir la garra
      abrirGarra();
    } else if (command.startsWith("GCLOSE")) {
      // Cerrar la garra
      cerrarGarra();
    }
  }
}

// Definir las funciones de movimiento y garra aquí
void moverEjeX(int direction) 
{
  if(direction == 1)
  {
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
    delay(200);
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
    delay(200);
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
    delay(200);
  }
  else if(direction == -1)
  {
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    delay(500);
  }
}
void moverEjeY(int direction) { /* Código para mover el eje Y */ }
void moverEjeZ(int direction) { /* Código para mover el eje Z */ }
void abrirGarra() { /* Código para abrir la garra */ }
void cerrarGarra() { /* Código para cerrar la garra */ }