
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

#ifndef _CLIENT_COMMUNICATION_h

#define _CLIENT_COMMUNICATION_h



//-------------------------------------------------------------------------------
//------------------------------LIBRARIES ---------------------------------------
//-------------------------------------------------------------------------------

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include<string>


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

#include "gbn.h"


#define SERVER_PORT 8000 //this is the udp port used in our system.
#define SERVER_HOST (char*)"localhost"
#define FAMILY_SOCKET AF_INET
#define TYPE_SOCKET SOCK_DGRAM
#define PROTOCOL_SOCKET IPPROTO_UDP
#define BUFFER_PACKET_SIZE 1000
#define LOSS_PROBABILITY 1e-2    		/* loss probability                            */
#define CORRUPTION_PROBABILITIY 1e-3    /* corruption probability                      */
/*
Communication
*/

//






class  Client_communication {
private:
char communication_string[BUFFER_PACKET_SIZE];
int local_socket;
unsigned int client_size;
struct sockaddr_in server;
struct sockaddr_in client;
exception server_address(char *server_addr,int server_port);
exception socket_create();
public: 
	Client_communication();
	exception send(char * message);
	int receive(char *message, int message_buffer);
  int receive(gbnp *packet,int lenght); 
  exception maybe_send(gbnp packet);

};








#endif






