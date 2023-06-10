#include <SoftwareSerial.h>

// PIN 11 -> RX de Arduino
// PIN 12 -> TX de Arduino
SoftwareSerial BTSerial(11, 12);   // RX | TX
char msg_recibido = ' ';
String msg;


void setup()
{
  Serial.begin(9600);
  Serial.println("Inicializando configuracion del HC-05...");

  BTSerial.begin(9600);
  Serial.println("Esperando Comandos AT...");
}

void loop()
{

  if (BTSerial.available())
  {
    Serial.println("Leemos del BT.");
    msg_recibido = BTSerial.read();
    Serial.println(msg_recibido);
    if (msg_recibido == '1')
    {
      Serial.println("Leeimos el 1.");
    } else {
      Serial.println("Leimos algo distinto de 1.");
    }
  }

  if (Serial.available())
  {
    Serial.println("Entramos al Serial.available");
    msg_recibido = Serial.read();
    BTSerial.write(msg_recibido);
  }
}