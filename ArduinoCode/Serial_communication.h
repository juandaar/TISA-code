
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

#ifndef _SERIAL_COMMUNICATION_h //check if this library was defined

#define _SERIAL_COMMUNICATION_h//defined if it is necessary 

//From computer to arduino example
//INIT_PACKAGE GET 12 TEMPERATURE END_PACKAGE


//From arduino to computer example
//INIT_PACKAGE OK 14 TEMPERATURE 24 END_PACKAGE


/*
 * the objective with this library is to create  a space that permits developing a good practices in the communication 
 * between an arduino and a computer when the objetives is to  capture  sensor values using a serial communication
 */

//   ----------------------------------------------------------------------------
//   -------------------------- INCLUDES  ---------------------------------------
//   ----------------------------------------------------------------------------

#include <avr/wdt.h> //this library was used for stop the watchdog.
#include <Stream.h>
#include<DHT11.h> //this library allows understand the  information receiving  from the  DHT11 sensor and it converts the receiving information  in humidity and temperature value

//   ----------------------------------------------------------------------------
//   -------------------------- DEFINITIONS -------------------------------------
//   ----------------------------------------------------------------------------
#define INIT_PACKAGE "INIT_PACKAGE"
#define END_PACKAGE "END_PACKAGE"
#define BUFFER_SIZE 200 
#define MAXIMUM_SENSOR_VALUE_STRING 24


//typedef enum communication represents the possible answers that the arduino can do  to the computer
typedef enum communication{
  OK, //all is ok 
  FAIL, //some problem with the communication 
  HI,  //hi is really the HELLO answer
  WAIT //it is not defined.
} communication;

//typedef enum communication_package represents the possible packets that the computer can send to the arduino 
typedef enum communication_package {
  GET, //when the computer needs receive some information
  POST,//when the computer needs change some configuration (does not work)
  HELLO,//when the computer needs to check if the arduino serial  is alive.
  ERR,//when the communication has a mistake
}communication_package;

//typedef enum sensor_type represents the possible sensor values that the arduino can collect.
typedef enum sensor_type{
  TEMPERATURE, //dht11 sensor
  HUMIDITY,    //dht11 sensor
  TEMPERATURE_AND_HUMIDITY, //dht11 sensor
  NON //sensor does not define
}sensor_type;

//typedef enum exception represents the possible error in the arduino system
typedef enum exception {
  EXE_OK, //all is ok
  EXE_ERR //a mistake happened
}exception;


//   ----------------------------------------------------------------------------
//   -------------------------- CLASS DEFINITION---------------------------------
//   ----------------------------------------------------------------------------
class Serial_communication {
  
//   -------------------------- PRIVATE-----------------------------------------
  private:
   Stream  &m_stream; //this stream reference received the Serial stream
   DHT11   *m_dht11;  //this pointer received the dht11 memory direction 
   communication_package package;//the last communication package 
   communication next_package; //the next communication state
   sensor_type sensor_kind; //the last sensor value needed
   String last_sensor_value;//Information string
   exception sensor_value(); //return the sensor value
   exception read_package(String message); //reads the last   received  packagem
   exception send(); //send a package
   exception read(); //listen in the serial port 
   
//   -------------------------- PUBLIC-----------------------------------------
public:
  Serial_communication(Stream &stream,DHT11 *dht11); //builder function that received the Serial stream reference and the dht11 memory direction
  void loop(); //loop function 
};

#endif


