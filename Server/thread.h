
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

#include "communication.h"
// Threads Library
#include <pthread.h>
#include<string>
// Timers library
#include <time.h>
#include "exception.h"
#include "gbn.h"

#define BUFFER_CLIENT 5
#define BUFFER_PACKET_WAIT 100 //buffer packet received
#define FILE_NAME "DATA_BASE.csv"


     
      
      

class Threads {
	private:
 //-------------------------------------------------------------------------------
    //------------------------------PTHREADS DECLARATION-----------------------------
    //-------------------------------------------------------------------------------
    static pthread_t Communication_thread; //this thread is used by client as communication managem between client to server
    static pthread_t Listen_thread;
    //------------------------------pthreads mutex declaration-----------------------
    static pthread_mutex_t  Database_mutex;  //This mutex is used as a buffer client protector
    static communication *m_Socket;
    static void * Server_thread(void*); //this made a server logical
    static void * Server_listen_thread(void*); //this made as a blocking receptor
    static int init;
    static server_packet packets[BUFFER_PACKET_WAIT];
    static int buffer_packets_count; //quantity of buffer spaces full
    static int init_bussy_buffer;//when start the bussybuffer
    static int end_bussy_buffer;//when end the bussy buffer
    static pthread_mutex_t  Packets_mutex; //this mutes is used as a packets information protector
    static exception write_data_base(char *information);
	  public:
    Threads(communication *socket);
    void loop();

};


#endif

