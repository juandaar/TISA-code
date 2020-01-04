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

#include "gbn.h"



    uint16_t checksum(gbnp *packet)
{
  int lenght=(sizeof(packet->type) + sizeof(packet->seqnum) + sizeof(packet->data))/sizeof(uint16_t);
    uint16_t buffer_array[lenght];
    buffer_array[0]=(uint16_t)packet->seqnum + ((uint16_t)packet->type << 8);
  for (int byte_index = 1; byte_index <= sizeof(packet->data); byte_index++){
    int word_index = (byte_index + 1) / 2;
    if (byte_index % 2 == 1){
      buffer_array[word_index] = (uint8_t)packet->data[byte_index-1];
    } else {
      buffer_array[word_index] = buffer_array[word_index] << 8;
      buffer_array[word_index] += (uint8_t)packet -> data[byte_index - 1];
    }
  }
    uint16_t *buf = buffer_array;
  uint32_t sum;
  for (sum = 0; lenght > 0; lenght--)sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
   return  ~sum;
}

int new_timeout(int time_out,int packet_time, int packet_state){
    int new_time=0;
      if(packet_state==0){new_time=(1-RTT_CONSTANT)*time_out+RTT_CONSTANT*(time_out+5);} //if the timeout packet does not receive the "new timeout is +5 miliseconds"
      else new_time=(1-RTT_CONSTANT)*time_out+RTT_CONSTANT*packet_time;
    if(new_time<200) return new_time=200;
    if(new_time>500) return new_time=500;
    return new_time;
  }



exception gbn_data_packet(char *information,size_t information_size,gbnp *packet,uint8_t *sequence){
  packet->type=DATA;
  packet->seqnum=*sequence;
  *sequence=*sequence+1;
  memset(packet->data,'\0', sizeof(*packet->data));
  int max_buffer=DATA_LENGTH;
  if(max_buffer<information_size) {
    printf("The packet is too big\n");
    return EXE_ERR;
  }
  sprintf(packet->data,"%s",information);
  packet->checksum=checksum(packet);
  return EXE_OK;
}

exception gbn_dataACK_packet (gbnp *data_packet,gbnp *packet,uint8_t *sequence) {
  packet->type=DATAACK;
  packet->seqnum=packet->seqnum;
  *sequence=*sequence+1;
  memset(packet->data,'\0', sizeof(*packet->data));
  sprintf(packet->data,"\n");
  packet->checksum=checksum(packet);
  return EXE_OK;
}


