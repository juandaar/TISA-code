
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



#ifndef _CLIENT_THREAD_h

#define _CLIENT_THREAD_h


#include "Serial_communication.h" //this library is charge to do the communication from user to arduino
#include "Client_communication.h"
#include "swing_door.h"
// Threads Library
#include <pthread.h>
#include<string>
// Timers library
#include <time.h>
#include "exception.h"
#include "gbn.h"


#define BUFFER_CLIENT 5
#define BUFFER_CLIENT_STRING 500
#define TEMPERATURE_RANGE 1
#define HUMIDITY_RANGE 1
#define TIME_VALUE 3
     


      

class Threads {
	private:
 //-------------------------------------------------------------------------------
    //------------------------------PTHREADS DECLARATION-----------------------------
    //-------------------------------------------------------------------------------
    static pthread_t Serial_client_thread; //this thread is used by client as managem the serial communication with an arduino
    static pthread_t Client_communication_thread; //this thread is used by client as communication managem between client to re
    static pthread_t Listen_thread; //this thread is used by receive message thread
    //------------------------------pthreads mutex declaration-----------------------
    static pthread_mutex_t  Client_buffer_mutex;  //This mutex is used as a buffer client protector
    static pthread_mutex_t  Packets_mutex; //this mutes is used as a packets information protector
    
        //------------------------------pthreads condition declaration-----------------------
    static pthread_cond_t Server_communication_condition; //when the system has information to send 
    static pthread_cond_t Critical_buffer_condition;//when the buffer is full
    static sensor_structure client_buffer[2][BUFFER_CLIENT];//sensor double buffer (the objetive is to try does not lost information when something happens)
    static sensor_structure temporal_client_value;
    static client_packet packets[BUFFER_CLIENT]; //it see like a complete buffer is a complete message but it is automatically created, more end packet
    static int buffer_state;//checks if the last informations that need to send to the server was sended or not
    static int buffer_used;//buffer used 0 or 1
    static int buffer_count;//count the buffer space used in this moment
    static int receiver_state; //when the system does not need receive something, it is 0
    static int alert_sistem;//when the arduino does not answer anything for long time, the client notifies the server (future developments)
    static Serial_communication *m_Serial; //this pointer receive the object memory direction to use the serial_communication methods
    static Client_communication *m_Socket;//this pointer receive the object memory direction to use the client_communication methods
    static Swing  Temperature;
    static Swing  Humidity;
    static exception Sensor_structureToChar(sensor_structure structure,char *buffer,int *information_size);
    static void * Arduino_thread(void*); //this function is used for the Serial communication management and the information compilation, 
                                         //additionally it is used for notification that it has the necessary information 
                                         //to make sending information using the UDP socket
    static void * Client_thread(void*);  //this function is used for the  UDP communication management. 
    static void * Client_listen_thread(void*);//this function is used to receive packets
    static int init;
    public:
    Threads(Serial_communication *serial, Client_communication *socket);
    void loop();

};






#endif

