 
/*
Santa Catarina Federal University  "Universidad Federal de santa catarina " UFSC 

April 2018 - Semester 01 and bimester 01.

Curse:  
  Técnicas de Implementação de sistemas Automatizados.

Program:
  Mestrado em Engenharia de Automação e systemas

Teacher: 
  Dr. Carlos Barros Montez

Creating for:
  Juan David Arias Correa
  Hernesto Alfarato Ruiz Urrutia 
  Mario Palma Alvarez


*/
//   ----------------------------------------------------------------------------
//   -------------------------- INCLUDES  ---------------------------------------
//   ----------------------------------------------------------------------------
#include<DHT11.h> //this library allows understand the  information receiving  from the  DHT11 sensor and it converts the receiving information  in humidity and temperature values
#include"Serial_communication.h" //this library was created to be used as  a simple and rustic communication protocol for the sistem arduino-computer.

//   ----------------------------------------------------------------------------
//   -------------------------- DEFINITIONS  ------------------------------------
//   ----------------------------------------------------------------------------
#define DHT11_PIN 4 //defines the pin used it to receive the information.
DHT11 *dht= new DHT11(DHT11_PIN); //create a object dht11
Serial_communication *serial_communication; //created a pointer serial_communication;




void setup() {
  Serial.begin(19200); //serial begin
  serial_communication = new Serial_communication(Serial,dht); //builder of serial_communication library
}


void loop() {
  serial_communication->loop(); //it is a blocked communication system
}
