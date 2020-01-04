
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


#include "Client_thread.h"




    Threads::Threads (Serial_communication *serial, Client_communication *socket)
    { 
    if(init==0) {
     m_Serial=serial;
     m_Socket=socket;
     buffer_count=0;
     buffer_state=0;
     buffer_used=0;
     temporal_client_value.state=NON;
     printf("The Serial communication  and Socket communication object pointer were  correctly referenced\n");
     pthread_create(&Serial_client_thread,NULL,Arduino_thread,NULL);
     pthread_create(&Client_communication_thread,NULL,Client_thread,NULL);
     pthread_create(&Listen_thread,NULL,Client_listen_thread,NULL);
     printf("The pthreads were  correctly created\n");
     init=1;
     }
     else printf("Only can exist one Threads object in the same time\n");
    }
    
    void Threads::loop(){
      while(1){ 
      }
      /*
     printf("The pthreads were  correctly jointed\n");
     pthread_join(Serial_client_thread,NULL);
     pthread_join(Client_communication_thread,NULL);*/
    }

    exception Threads::Sensor_structureToChar(sensor_structure structure,char *buffer,int *information_size){
      sprintf(buffer,"%s;%s;%f;%f\n",structure.date,structure.time,structure.temperature,structure.humidity);
      int i=0;
      while(buffer[i]!='\n')i++;
      *information_size=i++;
      return EXE_OK;
    }


    void * Threads::Arduino_thread(void*){
      sleep(5);
      sensor_structure temp_structure;
      exception temperature_exception;
      exception humidity_exception;
      int save=0;
      while(1) {
       sleep(1);
       temp_structure=m_Serial->get(TEMPERATURE_AND_HUMIDITY);
      while(temp_structure.state==NON)  {sleep(1);  temp_structure=m_Serial->get(TEMPERATURE_AND_HUMIDITY);}
      printf("Received value from Arduino:date:%s time:%s temperature:%f humidity:%f \n",temp_structure.date,temp_structure.time,temp_structure.temperature,temp_structure.humidity);
      if(temporal_client_value.state==NON){
        if(Temperature.check(temp_structure.temperature)== EXE_WAIT &&  Humidity.check(temp_structure.humidity)==EXE_WAIT) temporal_client_value=temp_structure;
      }
      else {
        temperature_exception=Temperature.check(temp_structure.temperature);
        humidity_exception   =Humidity.check(temp_structure.humidity);
        if(temperature_exception==EXE_OK && humidity_exception==EXE_OK) save=1;
        else if(temperature_exception==EXE_OK) {
          Humidity.synchronization(temp_structure.humidity);
          save=1;

        }
        else if(humidity_exception==EXE_OK) {
          Temperature.synchronization(temp_structure.temperature);
          save=1;
        }

      }
 
      if(save==1) {
        printf("value saved in the base data:date:%s time:%s temperature:%f humidity:%f \n",temporal_client_value.date,temporal_client_value.time,temporal_client_value.temperature,temporal_client_value.humidity);
        client_buffer[buffer_used][buffer_count]=temporal_client_value;
        temporal_client_value=temp_structure;
        save=0;
        buffer_count++;
      /*This is the critical situation*/
      if(buffer_count==BUFFER_CLIENT) {
        //memory full critical situation
        pthread_mutex_lock(&Client_buffer_mutex);
        if(buffer_state==1) {
          usleep(10000);
          pthread_cond_signal(&Server_communication_condition);
          pthread_mutex_unlock(&Client_buffer_mutex); 
          pthread_mutex_lock(&Client_buffer_mutex);
          printf("The system does not have more space for more information, needs to wait that the server answered for continue with this process\n");
          pthread_cond_wait(&Critical_buffer_condition,&Client_buffer_mutex);
        }    
        buffer_count=0;
        buffer_used=(buffer_used+1)%2;
        buffer_state=1;
        pthread_mutex_unlock(&Client_buffer_mutex); 
      }
      }
      if(buffer_state==1){
        pthread_mutex_lock(&Client_buffer_mutex);
        usleep(10000);
        pthread_cond_signal(&Server_communication_condition);
        pthread_mutex_unlock(&Client_buffer_mutex); 
      }
      
      }
      
     }




    void * Threads::Client_thread(void*){
      //sleep(5);
      int buffer_state_temp=1;
      int buffer_full=0;
      char buffer_information[BUFFER_CLIENT_STRING];
      struct timeval actual_time;
      long int microseconds;
      int packet_time=0; //last packet time between sent and receive;
      int diference=0;
      uint8_t conditional_sequence_number=0;
      int mistery_error=0;
      while(1) 
      {
              pthread_mutex_lock(&Client_buffer_mutex);
              buffer_state=0; //buffer state=0;
              pthread_cond_signal(&Critical_buffer_condition);
              pthread_mutex_unlock(&Client_buffer_mutex);

              pthread_mutex_lock(&Client_buffer_mutex);
              pthread_cond_wait(&Server_communication_condition,&Client_buffer_mutex);
              buffer_state_temp=buffer_state;
              usleep(50000);
              pthread_mutex_unlock(&Client_buffer_mutex);
              
              if(sequence_number>200)sequence_number=1; //sequence number control
              while(buffer_state_temp==1) 
              {
                pthread_mutex_lock(&Packets_mutex);
                receiver_state=1;
                buffer_full=(buffer_used+1)%2;
                int length=0;
                //create all DATA packets
                for(int i=0;i<BUFFER_CLIENT;i++)
                {
                 if(Sensor_structureToChar(client_buffer[buffer_full][i],buffer_information,&length)!=EXE_OK) printf("Problem with the structure to char convertion\n");
                  packets[i].state=INIT;
                  packets[i].windows=NEED;
                  packets[i].length=length;
                  packets[i].time_ACK=0;
                  conditional_sequence_number=sequence_number;
                  if(gbn_data_packet(buffer_information,length,&packets[i].DATA_packet,&conditional_sequence_number)!=EXE_OK) printf("problem with the gbn packet construction\n");
                  sequence_number=conditional_sequence_number;
                }
                 pthread_mutex_unlock(&Packets_mutex);
                //send packets 
                int all=0;
                //how many windows is been using now
                int sent_message_token_confirmed=0; //last packet confirmed
                int sent_message=0; //last packet sent
                int count=0; //counter control
                while(all==0)
                {

                  //printf("TIMEOUT:%d\n",timeout);
                  pthread_mutex_lock(&Packets_mutex);  
                  gettimeofday(&actual_time,NULL);
                  microseconds = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000; //timeout control
                  count=sent_message;
                  while(number_busy_windows<MAX_WINDOW_SIZE && count<BUFFER_CLIENT)  //check if exist free windows and check if some packet needs to will send
                  {
                    
                    if(packets[count].windows==NEED) //check if this packet waits a windows  
                    {
                        m_Socket->maybe_send(packets[count].DATA_packet); //send a packet
                        printf("First packet sent time_out:%d,",timeout);
                        printf("type:%d,seqnum:%d,checksum:%d,data:%s",packets[count].DATA_packet.type,packets[count].DATA_packet.seqnum,packets[count].DATA_packet.checksum,packets[count].DATA_packet.data);
                        number_busy_windows++;
                        pthread_mutex_unlock(&Packets_mutex);
                        usleep(TIMEBETWEENPACKETS);
                        pthread_mutex_lock(&Packets_mutex);
                        gettimeofday(&actual_time,NULL);
                        microseconds = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000; //timeout control
                        packets[count].windows=BEING; //in this moment it used a windows
                        packets[count].time_sent=microseconds; //saves when the packet was sent
                        packets[count].state=SENT; //notifice that the packet was sent
                        sent_message++;
                      
                    
                    } 
                    count++;       
                  }
                  count=sent_message_token_confirmed;
                 // printf("confirmed:%d\n",count);
                  while(count<BUFFER_CLIENT) //check message state
                  {
                  
                    gettimeofday(&actual_time,NULL);
                    microseconds = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000; //timeout control
                    if(packets[count].state==SENT) //if the menssage was SENT is important
                    {
                    
                      if(packets[count].time_ACK!=0) 
                      {
                        diference=packets[count].time_ACK-packets[count].time_sent;
                        if(diference>timeout)
                        {
                            packet_time=(packets[count].time_ACK-packets[count].time_sent);
                            timeout=new_timeout(timeout,packet_time,1);
                            total_attempts++;
                            attempts++;
                            if(attempts==MAX_ATTEMPTS) {
                            //  printf("Alert: maximum attempts exceeded.As it is a example he try forever. ");
                            // printf("Number of attemps:%d \n",total_attempts);
                              attempts=0; 

                            }
                          if(packets[count].windows==BEING) //if one packet is lost every packet after that in the windows, they need to be sent again. 
                          {
                            printf("Timeout end, packet forwarded :time out:%d,",timeout);
                            printf("type:%d,seqnum:%d,checksum:%d,data:%s\n",packets[count].DATA_packet.type,packets[count].DATA_packet.seqnum,packets[count].DATA_packet.checksum,packets[count].DATA_packet.data);
                              m_Socket->maybe_send(packets[count].DATA_packet);
                              pthread_mutex_unlock(&Packets_mutex);
                              usleep(5000); //5 microseconds
                              pthread_mutex_lock(&Packets_mutex);
                              gettimeofday(&actual_time,NULL);       
                              microseconds = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000; //timeout control
                              packets[count].time_sent=microseconds; //saves when the packet was sent
                              packets[count].time_ACK=0;
                             // packets[count].state=SENT; //notifice that the packet was sent
                            
                          }
                          
                        }

                      }
                      else if((microseconds-packets[count].time_sent)>timeout) 
                      {  
                        timeout=new_timeout(timeout,packet_time,0);
                        total_attempts++;
                        attempts++;
                        if(attempts==MAX_ATTEMPTS) 
                        {
                            //  printf("Alert: maximum attempts exceeded.As it is a example he try forever. ");
                             // printf("Number of attemps:%d \n",total_attempts);
                            attempts=0; 
                        }
                        if(packets[count].windows==BEING) //if one packet is lost every packet after that in the windows, they need to be sent again. 
                          {
                              printf("ACK not received, Packet forwarded:time out:%d,",timeout);
                              printf("type:%d,seqnum:%d,checksum:%d,data:%s\n",packets[count].DATA_packet.type,packets[count].DATA_packet.seqnum,packets[count].DATA_packet.checksum,packets[count].DATA_packet.data);
                              m_Socket->maybe_send(packets[count].DATA_packet); //send a packet
                              pthread_mutex_unlock(&Packets_mutex);
                              usleep(5000); //5 microseconds
                              pthread_mutex_lock(&Packets_mutex);
                              gettimeofday(&actual_time,NULL);
                              microseconds = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000; //timeout control
                              packets[count].time_sent=microseconds; //saves when the packet was sent
                              packets[count].time_ACK=0;
                              //packets[count].state=SENT; //notifice that the packet was sent
                             
                          }

                          
                      }
                       
                    } /*else if(packets[count].state==INIT)count=BUFFER_CLIENT;*/
                      else if(packets[count].state==CORRECT) sent_message_token_confirmed++;
                  
                    count++;
  
                  }
                    if(packets[BUFFER_CLIENT-1].state==CORRECT || sent_message_token_confirmed==(BUFFER_CLIENT)) {
                      buffer_state_temp=0; all=1; receiver_state=0;
                    }
                  pthread_mutex_unlock(&Packets_mutex);
                }
              }

              
    }
 }



  void * Threads::Client_listen_thread(void*)
  { 
    gbnp packet;
    int bytes;
    int count=0;
    long int microseconds;
    struct timeval actual_time;
    int packet_time=0; //last packet time between sent and receive;
    while(1){
      bytes=m_Socket->receive(&packet,sizeof(packet));
      printf("Packet arrived: ");
      printf("type:%d,seqnum:%d,checksum:%d,data:%s\n",packet.type,packet.seqnum,packet.checksum,packet.data);
      gettimeofday(&actual_time,NULL);
      microseconds = actual_time.tv_sec * 1000 + actual_time.tv_usec / 1000; //timeout control
      if(bytes>0) {
        if(packet.type==DATAACK && packet.checksum==checksum(&packet))
        {  
          printf("The ACK packet with seqnum:%d arrived\n",packet.seqnum);
          pthread_mutex_lock(&Packets_mutex);
          if(receiver_state==1)
          {
            if(number_busy_windows>0) 
            {

              count=0;
              int diference=0;
              while(count<BUFFER_CLIENT) //check off packets 
              {
                if(packets[count].state==SENT &&packets[count].DATA_packet.seqnum==packet.seqnum) //check if the packet was sent and if this seqnum is equal than its seqnum
                {      
                  packets[count].time_ACK=microseconds;
                  diference=packets[count].time_ACK-packets[count].time_sent;
                  if(diference<=timeout)
                  {
                    int past_time=count; //check if the timeout the other packet was overcome
                    int check_time_out=1; //if it is 1 all is good, but if it is 0 the packet is discarded
                    while(past_time>=0)
                    {
                      if(packets[past_time].state==SENT) 
                      {  
                         diference=microseconds-packets[past_time].time_sent;
                         if(diference>timeout ) check_time_out=0;
                      }
                      else past_time=0;
                      past_time--;
                    }
          
                    if(check_time_out==1) //the ACK accept and this affect all past packets  
                    {
                    
                      past_time=count;
                      int packet_time=microseconds-packets[count].time_sent;
                      timeout=new_timeout(timeout,packet_time,1);
                      while(past_time>=0){
                        if(packets[past_time].state==SENT) {
                          packets[past_time].time_ACK=microseconds;
                          packets[past_time].DATAACK_packet=packet;
                          packets[past_time].state=CORRECT;
                          packets[past_time].windows=USED; 
                          number_busy_windows--;
                        }
                        else past_time=0;
                      past_time--;
                      }

                    }else   
                    {
                      past_time=count;
                      while(past_time>=0){
                        if(packets[past_time].state==SENT) {
                          packets[past_time].time_ACK=0;
                        }
                        else past_time=0;
                      past_time--;
                      }

                    }

                  }
                  count=BUFFER_CLIENT;
              } 
              count++;
            }
          }
        }
          pthread_mutex_unlock(&Packets_mutex);
        }
      }
    }
    
  }






 
    int Threads::init=0;
    /*
    Static variable
    */
    Serial_communication *  Threads::m_Serial;    
    Client_communication *  Threads::m_Socket;
    int                     Threads::buffer_state=0;
    int                     Threads::buffer_used=0;
    int                     Threads::buffer_count=0;
    int                     Threads::alert_sistem=0;
    int                     Threads::receiver_state=0;
    sensor_structure        Threads::client_buffer[2][BUFFER_CLIENT];
    pthread_mutex_t         Threads::Client_buffer_mutex=  PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t         Threads::Packets_mutex=  PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t          Threads::Server_communication_condition= PTHREAD_COND_INITIALIZER;
    pthread_cond_t          Threads::Critical_buffer_condition= PTHREAD_COND_INITIALIZER;
    pthread_t               Threads::Serial_client_thread;
    pthread_t               Threads::Client_communication_thread;
    pthread_t               Threads::Listen_thread;
    sensor_structure        Threads::temporal_client_value;
    Swing                   Threads::Temperature(TEMPERATURE_RANGE,TIME_VALUE);
    Swing                   Threads::Humidity(HUMIDITY_RANGE,TIME_VALUE);
    client_packet           Threads::packets[BUFFER_CLIENT]; //it see like a complete buffer is a complete message but it is automatically created, more end packet













