
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



#ifndef _SWING_h

#define _SWING_h


#include "exception.h"
#include <stdio.h>


typedef enum range_type {
 INTEGER,
 PORCENTUAL
}range_type;
     
      
      

class Swing {
	  private:
    int current_value; //current value of the swing door
    int range; //range current_value +-range
    int discrete_time; //how many times the swing need to be used for accept a new value
    int current_time;  //current time, current time <discrete time
    int first_time; //has a diferent behavior when receive a first value;
    range_type range_way;
	  public:
    Swing(float range_value,int time);
    Swing (float range_value,range_type tipe,int time);
    exception check(float value); //is used to check a external value with the present "door"
    exception synchronization(float value);

};


#endif

