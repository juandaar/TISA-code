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


#ifndef _GBN_h

#define _GBN_h

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<signal.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<time.h>
#include <stdbool.h>

#include "exception.h"

/*----- System parameters -----*/
#define MAX_ATTEMPTS      5 // The max attempts 

#define MAX_WINDOW_SIZE   3// The max windows size
#define INITIAL_TIME_OUT  200 //100 ms
#define RTT_CONSTANT      0.125
#define INITIAL_SEQUENCE_NUMBER 1


/*----- Protocol parameters -----*/
#define LOSS_PROBABILITY 1e-2       /* loss probability                            */
#define CORRUPTION_PROBABILITIY 1e-3    /* corruption probability                      */
#define DATA_LENGTH  512          /* length of the payload                       */
#define N         1024              /* Max number of packets a single call to send can process */
#define TIMEOUT      ((TIMEBETWEENPACKETS/1000)*(4)*(MAX_WINDOW_SIZE))                  /* timeout to resend packets (microsseconds)        */
#define TIMEBETWEENPACKETS 100000 /* 100 miliseconds in nanoseconds*/
#define ACTUAL_TIME 10
#define SURVIVER_PACKET 100000 /*100 seconds in miliseconds*/
#define SURVIVER_ACK  120000   /*120 seconds in miliseconds*/


/*----- Packet types -----*/
#define SYN      0        /* Opens a connection                          */
#define SYNACK   1        /* Acknowledgement of the SYN packet           */
#define DATA     2        /* Data packets                                */
#define DATAACK  3        /* Acknowledgement of the DATA packet          */
#define FIN      4        /* Ends a connection                           */
#define FINACK   5        /* Acknowledgement of the FIN packet           */
#define RST      6        /* Reset packet used to reject new connections */


typedef enum packet_state{
  INIT,
  SENT,
  CORRECT,
  LOST,
}packet_state;




typedef enum windows_state{
  NEED, //wait a turn
  BEING,//it is part of this windows
  USED, //it end correctly
}windows_state;

/*----- Go-Back-n packet format -----*/
typedef struct {
  uint8_t  type;                   /* packet type (e.g. SYN, DATA, ACK, FIN)     */
  uint8_t  seqnum;                /* sequence number of the packet              */
  uint16_t checksum;             /* header and payload checksum                */
  char data[DATA_LENGTH];    /* pointer to the payload                     */
} __attribute__((packed)) gbnp;



typedef struct client_packet{
  packet_state state;
  windows_state windows;
  uint8_t windows_number;
  gbnp    DATA_packet;
  gbnp    DATAACK_packet;
  int     length;
  double   time_sent; //when was sent
  double     time_ACK; //WHEN the ack return
} client_packet;

typedef struct server_packet{
  gbnp    DATA_packet;
  gbnp    DATAACK_packet;
  double time; //when the packet was received for first time
}server_packet;





uint16_t checksum(gbnp *packet); //checksum definition
int new_timeout (int time_out,int packet_time,int packet_state); //(actual time out, the time that the ACK return, (0 if the packet does not arrive before the timeout or 1 if the packet arrived good))
exception gbn_data_packet(char *information,size_t information_size,gbnp *packet,uint8_t *sequence);
exception gbn_dataACK_packet (gbnp *data_packet,gbnp *packet,uint8_t *sequence);
static uint8_t sequence_number=INITIAL_SEQUENCE_NUMBER;
static int number_busy_windows=0; //how many windows is used now
static int timeout=TIMEOUT; 
static int sequence_number_change=0;
static int total_attempts=0;
static int attempts=0;


#endif

