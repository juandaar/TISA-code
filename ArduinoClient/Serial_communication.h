
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

#define _SERIAL_COMMUNICATION_h


// Basic Libraries
#include <ctime>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
//#include <time.h>
// Serial Ports Libraries
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/times.h>

#include "exception.h"


/*
 * the objective with this library is to create  a space that permits developing a good practices in the communication 
 * between an arduino and a computer when the objetives is to  capture  sensor values using a serial communication
 */
#define INIT_PACKAGE "INIT_PACKAGE"
#define END_PACKAGE "END_PACKAGE"
#define SERIAL_BUFFER_SIZE 200
#define BAUD_RATE B19200
#define PORT_NAME "/dev/ttyACM0"
#define TEMPERATURE_GET "INIT_PACKAGE GET 13 TEMPERATURE END_PAWCKAGE"
#define HUMIDITY_GET "INIT_PACKAGE GET 10 HUMIDITY END_PACKAGE"
#define TEMPERATURE_AND_HUMIDITY_GET "INIT_PACKAGE GET 26 TEMPERATURE_AND_HUMIDITY END_PACKAGE"
#define HELLO_MESSAGE "INIT_PACKAGE HELLO END_PACKAGE"
#define HELLO_ANSWER "INIT_PACKAGE OK HELLO END_PACKAGE"

typedef enum serial_communication{
  OK, //all is good
  FAIL, //problem
  HI, //it is not used now
  WAIT  //it is not used now
} serial_communication;

typedef enum serial_communication_package {
  GET, //when the computer needs receive some information
  POST,//when the computer needs change some configuration (does not work)
  HELLO,//when the computer needs to check if the arduino serial  is alive.
  ERR,//when the communication has a mistake
}serial_communication_package;


typedef enum sensor_type{
  TEMPERATURE, //TEMPERATURE VALUE
  HUMIDITY,  //HUMIDITY VALUE
  TEMPERATURE_AND_HUMIDITY,//Temperature and humidity values
  NON  //it does not have information
}sensor_type;



typedef struct sensor_structure {
sensor_type state;
char  date[20];
char  time[20];
float temperature;
float humidity;
} sensor_structure;


class Serial_communication {
private:
int serial_file;
sensor_structure sensor_values;
exception hello(); //checks if the connection is well
exception package(char *payload,int payload_size); //it unpacks the arduino response
void  StringDate(char *date,char *time); //it is used to return the date and time in char * format
public:
Serial_communication();  //builder
sensor_structure  get(sensor_type); //it returns a sensor_structure that has the sensor values that send a arduino (in this case only temperature, humidity or both)
};

#endif






