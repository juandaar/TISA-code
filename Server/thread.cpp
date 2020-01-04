
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


#include "thread.h"




    Threads::Threads (communication *socket)
    { 
    

    if(init==0) {
     m_Socket=socket;
     printf("The Serial communication  and Socket communication object pointer were  correctly referenced\n");
     pthread_create(&Communication_thread,NULL,Server_thread,NULL);
     pthread_create(&Listen_thread,NULL,Server_listen_thread,NULL);
     printf("The pthreads were  correctly created\n");
     init=1;
     }
     else printf("Only can exist one Threads object in the same time\n");
    }
    
    void Threads::loop(){
     while(1){

     }

    }



    void * Threads::Server_thread(void*){
    
      long int microseconds;
       struct timeval actual_time;
       int count=0;
       int last_time=0; //0 this package expired , 1 this is the first package that not expired
       int timer=0; //how many times happens
      while(1) {
        gettimeofday(&actual_time,NULL);
        microseconds = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000; //time control
        //              
        if(buffer_packets_count>0){
             //sent packets
              pthread_mutex_lock(&Packets_mutex); 
              if(end_bussy_buffer<init_bussy_buffer)
              {
               
                 count=init_bussy_buffer;
                while(count<BUFFER_PACKET_WAIT){
                  
                  if(packets[count].state==INIT ||packets[count].state==LOST)
                  {
                   pthread_mutex_unlock(&Packets_mutex);
                   usleep(TIMEBETWEENPACKETS);
                   pthread_mutex_lock(&Packets_mutex);
                   gettimeofday(&actual_time,NULL);
                   microseconds = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000; //time control
                   packets[count].state=SENT;
                   m_Socket->maybe_send(packets[count].DATAACK_packet); //send a packet

                   printf("ACK packet sent:  ");
                   printf("type:%d,seqnum:%d,checksum:%d,data:%s\n",packets[count].DATAACK_packet.type,packets[count].DATAACK_packet.seqnum,packets[count].DATAACK_packet.checksum,packets[count].DATAACK_packet.data);          

                  }
                  count++;
                }
                
                count=0;
                while(count<=end_bussy_buffer)
                {
                 if(packets[count].state==INIT ||packets[count].state==LOST)
                 {
                   pthread_mutex_unlock(&Packets_mutex);
                   usleep(TIMEBETWEENPACKETS);
                   pthread_mutex_lock(&Packets_mutex);
                   gettimeofday(&actual_time,NULL);
                   microseconds = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000; //time control
                   packets[count].state=SENT;
                   m_Socket->maybe_send(packets[count].DATAACK_packet); //send a packet
                   printf("ACK packet sent:  ");
                   printf("type:%d,seqnum:%d,checksum:%d,data:%s\n",packets[count].DATAACK_packet.type,packets[count].DATAACK_packet.seqnum,packets[count].DATAACK_packet.checksum,packets[count].DATAACK_packet.data);   
                  }
                  count++;
                }              

              }
              else
              {
                 count=init_bussy_buffer;
                 while(count<=end_bussy_buffer)
                 {
                  
                  timer=microseconds-packets[count].time;
                   if(packets[count].state==INIT ||packets[count].state==LOST){
                   pthread_mutex_unlock(&Packets_mutex);
                   usleep(TIMEBETWEENPACKETS);
                   pthread_mutex_lock(&Packets_mutex);
                   gettimeofday(&actual_time,NULL);
                   microseconds = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000; //time control
                   packets[count].state=SENT;
                   m_Socket->maybe_send(packets[count].DATAACK_packet); //send a packet
                   printf("ACK packet sent:  ");
                   printf("type:%d,seqnum:%d,checksum:%d,data:%s\n",packets[count].DATAACK_packet.type,packets[count].DATAACK_packet.seqnum,packets[count].DATAACK_packet.checksum,packets[count].DATAACK_packet.data);   
                 }
                 
                 count++;
                } 
              }  
              count=0;

              //Discarted packets for live time end
              gettimeofday(&actual_time,NULL);
              microseconds = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000; //time control
              last_time=0;
              if(end_bussy_buffer<init_bussy_buffer){
                
                 count=init_bussy_buffer;
                 while(count<BUFFER_PACKET_WAIT  && last_time==0){
                  timer=microseconds-packets[count].time;
          
                  if(timer<=SURVIVER_PACKET  ) {last_time=1;}
                  else 
                  {
                   buffer_packets_count=buffer_packets_count-1;
                   printf("The packet with seqnum:%d was deleting of the databases, because its live time was ended\n",packets[init_bussy_buffer].DATA_packet.seqnum);
                   init_bussy_buffer=init_bussy_buffer+1;
                   if(init_bussy_buffer==BUFFER_PACKET_WAIT) init_bussy_buffer=0;
                  }

                 count++;
                }
                count=0;
                 while(count<=end_bussy_buffer  && last_time==0){
                  timer=microseconds-packets[count].time;
     
                  if(timer<=SURVIVER_PACKET ) {last_time=1;}
                  else 
                  {
                   buffer_packets_count=buffer_packets_count-1;
                   printf("The packet with seqnum:%d was deleting of the databases, because its live time was ended\n",packets[init_bussy_buffer].DATA_packet.seqnum);
                   init_bussy_buffer=init_bussy_buffer+1;
                   if(init_bussy_buffer==BUFFER_PACKET_WAIT) init_bussy_buffer=0;
                  }
                 count++;
                }               

              }
              else {
               
                 count=init_bussy_buffer;
                 while(count<=end_bussy_buffer  && last_time==0){
                  timer=microseconds-packets[count].time;
     
                  if(timer<=SURVIVER_PACKET ) {last_time=1;}
                  else 
                  {
                   buffer_packets_count=buffer_packets_count-1;
                   printf("The packet with seqnum:%d was deleting of the databases, because its live time was ended\n",packets[init_bussy_buffer].DATA_packet.seqnum);
                   init_bussy_buffer=init_bussy_buffer+1;
                   if(init_bussy_buffer==BUFFER_PACKET_WAIT) init_bussy_buffer=0;
                  }
                 count++;
                }   

              }
          pthread_mutex_unlock(&Packets_mutex);
        }
        pthread_mutex_lock(&Packets_mutex);
        if(buffer_packets_count==0){
          init_bussy_buffer=0;
          end_bussy_buffer=-1;
        } 
        pthread_mutex_unlock(&Packets_mutex);
      }
    }



   



    void *Threads::Server_listen_thread(void*){
    //for(int i=0;i<BUFFER_PACKET_WAIT;i++) packets[i].buffer_state=FREE;
    gbnp packet;
    int bytes;
    int count=0;
    long int microseconds;
    struct timeval actual_time;
    uint8_t conditional_sequence_number=0;
    while(1){
      //susleep(50000);
      bytes=m_Socket->receive(&packet,sizeof(packet));
      gettimeofday(&actual_time,NULL);
      microseconds = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000; //time control
      if(bytes>0) {
        pthread_mutex_lock(&Packets_mutex);
        if(packet.type==DATA && packet.checksum==checksum(&packet)){  
          printf("The DATA packet with seqnum:%d arrived\n",packet.seqnum);
          if(buffer_packets_count<BUFFER_PACKET_WAIT || (packet.seqnum<sequence_number && buffer_packets_count==BUFFER_PACKET_WAIT)) //check the buffer space  
          {
            if(packet.seqnum==sequence_number ||(packet.seqnum==1 && sequence_number>200)) //check if the sequence number is correct 
            {
              if(packet.seqnum==1 && sequence_number>200) sequence_number=1; //sequence_number control
              //printf("First packet received: ");
              //printf("type:%d,seqnum:%d,checksum:%d,data:%s\n",packet.type,packet.seqnum,packet.checksum,packet.data);
              end_bussy_buffer=end_bussy_buffer+1;
              if(end_bussy_buffer==BUFFER_PACKET_WAIT) end_bussy_buffer=0;
              buffer_packets_count=buffer_packets_count+1;
              packets[end_bussy_buffer].DATA_packet=packet;
              packets[end_bussy_buffer].state=INIT;
              conditional_sequence_number=sequence_number;
              gbn_dataACK_packet(packet,&packets[end_bussy_buffer].DATAACK_packet,&conditional_sequence_number);
              sequence_number=conditional_sequence_number;
              write_data_base(packet.data);
              packets[end_bussy_buffer].time=microseconds;
            }

            else if(packet.seqnum<sequence_number && buffer_packets_count>0) //the ACK is share
            {
              int packet_find=0;//packet does not find
              int buffer_packet_find=0;//places where it is
              if(end_bussy_buffer<init_bussy_buffer)
              {
                 count=init_bussy_buffer;
                 while(count<BUFFER_PACKET_WAIT  && packet_find==0){
                 if(packets[count].DATA_packet.seqnum==packet.seqnum && packet.seqnum!=0) {
                  buffer_packet_find=count;
                  count=BUFFER_PACKET_WAIT;
                  packet_find=1;
                 }
                 count++;
                }
                count=0;
                 while(count<=end_bussy_buffer  && packet_find==0){
                 if(packets[count].DATA_packet.seqnum==packet.seqnum && packet.seqnum!=0) {
                  buffer_packet_find=count;
                  count=end_bussy_buffer;
                  packet_find=1;
                 }
                 count++;
                }               

              }
              else {

                count=init_bussy_buffer;
                while(count<=end_bussy_buffer && packet_find==0){
                 if(packets[count].DATA_packet.seqnum==packet.seqnum && packet.seqnum!=0) {
                  buffer_packet_find=count;
                  count=end_bussy_buffer;
                  packet_find=1;

                 }
                 count++;
                }

              }
              if(packet_find==0)  printf("The DATA packet is discarded, The sequence number does not exist in the internal buffer\n");
              else 
              {
                packets[buffer_packet_find].state=LOST;
              }
              

            }
            else  {
              printf("sequence_number:%dbuffer_packets_count:%d,init_bussy_buffer:%d,end_bussy_buffer:%d\n",sequence_number,buffer_packets_count,init_bussy_buffer,end_bussy_buffer);
              printf("The DATA packet is discarded, The sequence number does not have sense\n");
            }

          }
          else printf("The packet was lost, because the buffer does not have more space\n");
         pthread_mutex_unlock(&Packets_mutex); 
        }
      
      }
    }
    
    }



    exception Threads::write_data_base(char *information){
      int count=0;
      int i=0;
      while(information[i]!='\n' && i<DATA_LENGTH) {
        if(information[i]==';')count++;
         i++;
        }
        
        if(count!=3){
          printf("The information does not have the correctly format, for this reason was discarded\n");
          return EXE_ERR;
        }
      

     //char line_file[LINE_FILE_BUFFER_SIZE];
         FILE * file; //creating the file
       if (!(file = fopen(FILE_NAME, "r")))
         {
         file = fopen(FILE_NAME,"a");
          char initial_column[]="DATE;TIME;TEMPERATURE;HUMIDITY\n";
        fprintf(file,"%s",initial_column);
      fclose(file);
         }
      /*
            this function make or open a file that it respects the *.CSV format
      */
        file = fopen(FILE_NAME,"a");
        fprintf(file,"%s",information);    
        fclose(file);
        printf("The information was correctly saved in the data base\n");
      return EXE_OK;
    }


    int Threads::init=0;
    /*
    Static variable
    */
    communication *  Threads::m_Socket;
    pthread_mutex_t         Threads::Database_mutex=  PTHREAD_MUTEX_INITIALIZER;
    pthread_t               Threads::Communication_thread;
    pthread_t               Threads::Listen_thread;
    server_packet           Threads::packets[BUFFER_PACKET_WAIT];
    int                     Threads::buffer_packets_count=0;
    int                     Threads::init_bussy_buffer=0;//when start the bussybuffer
    int                     Threads::end_bussy_buffer=-1;//when end the bussy buffer
    pthread_mutex_t         Threads::Packets_mutex=  PTHREAD_MUTEX_INITIALIZER;


