
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


#include "Serial_communication.h"



Serial_communication::Serial_communication() {
  // Set configurations to serial port of linux ("/dev/ttyACM0") for communication con ARDUINO UNO board
  struct termios serial_structure;
  /* open serial port */
  serial_file = open(PORT_NAME, O_RDWR | O_NOCTTY);
  // wait for the Arduino to reboot
  usleep(3500000);
  // get current serial port settings
  tcgetattr(serial_file, &serial_structure);
  
  // set 19200 baud both ways 
  cfsetispeed(&serial_structure, BAUD_RATE);
  cfsetospeed(&serial_structure, BAUD_RATE);
  
  // 8 bits, no parity, no stop bits 
  serial_structure.c_cflag &= ~PARENB;
  serial_structure.c_cflag &= ~CSTOPB;
  serial_structure.c_cflag &= ~CSIZE;
  serial_structure.c_cflag |= CS8;
  // Canonical mode 
  serial_structure.c_lflag |= ICANON;
  // commit the serial port settings
  tcsetattr(serial_file, TCSANOW, &serial_structure);
  int count=0;
  while(hello()!=EXE_OK && count<5) count++;
  if(count<5) printf("Serial_communication object was correctly created\n");
  else printf("Serial_communication object was not correctly created\n");
}

sensor_structure  Serial_communication::get(sensor_type sensor) {
  
  if(sensor==TEMPERATURE) {char array_send[]=TEMPERATURE_GET; write(serial_file,array_send,sizeof(array_send));}
  if(sensor==HUMIDITY) {char array_send[]=HUMIDITY_GET; write(serial_file,array_send,sizeof(array_send));}
  if(sensor==TEMPERATURE_AND_HUMIDITY) {char array_send[]=TEMPERATURE_AND_HUMIDITY_GET; write(serial_file,array_send,sizeof(array_send));}
  int bytes=0;
  char payload[SERIAL_BUFFER_SIZE];
  bytes = read(serial_file,payload,SERIAL_BUFFER_SIZE); 
  payload[bytes]='\0';
  if(package(payload,sizeof(payload))==EXE_OK) {
  sensor_values.state=sensor;
  return sensor_values;
   }
   else {
      sensor_values.state=NON;
      printf("The arduino did not answer correctly\n");
      return sensor_values;
   }
}






exception Serial_communication::hello(){
  char MESSAGE[]=HELLO_MESSAGE;
  write(serial_file,MESSAGE,sizeof(MESSAGE));
  int bytes=0;
  char payload[SERIAL_BUFFER_SIZE];
  bytes = read(serial_file,payload,SERIAL_BUFFER_SIZE); 
  payload[bytes]='\0';
  if(bytes<0) {printf("The arduino did not answer hello correctly\n");return EXE_ERR;}
  for(int i=0;i<sizeof(HELLO_ANSWER)-1 && i<bytes;i++) if(payload[i]!=HELLO_ANSWER[i])  {
    printf("The arduino did not answer hello correctly\n");
    return EXE_ERR;
  }
  printf("The arduino answered hello correctly\n");
  return EXE_OK;
}


exception Serial_communication::package(char *payload,int payload_size){ 
 int check_point=0;
 char init[]=INIT_PACKAGE;
 char ended[]=END_PACKAGE;
 char message[payload_size];
 while(check_point<sizeof(init)-1){
  if(init[check_point]!=payload[check_point]) {printf("The arduino did not answer hello correctly\n");return EXE_ERR;}
  check_point++;
 }
 check_point++;
 int count=0;
 while(payload[check_point]!=' ' &&  count<5 && check_point<payload_size){ message[count]=payload[check_point];  check_point++; count++;}
 message[count]='\0';
 if(strcmp(message,"OK")==0)  {
  check_point++;
  count=0;
  int information_size=0;
  while(payload[check_point]!=' ' && count<2 && check_point<payload_size) {message[count]=payload[check_point]; check_point++; count++;}
  message[count]='\0';
  information_size=atoi(message)-2;
  if(information_size<=payload_size and information_size>0) {
    check_point++;
    count=0;
    while(payload[check_point]!=' ' && count<information_size && check_point<payload_size) {message[count]=payload[check_point]; check_point++; count++;}
    if(count>information_size) {printf("The arduino did not answer hello correctly\n");return EXE_ERR;}
    message[count]='\0';
    if(strcmp(message,"TEMPERATURE")==0) {
       check_point++;
       count=0;
       float temperature=0.0;
       while(payload[check_point]!=' '  && check_point<payload_size) {message[count]=payload[check_point]; check_point++; count++;}
       message[count]='\0';
       temperature=atof(message);
       sensor_values.temperature=temperature;
       sensor_values.humidity=0.0;
    }
    else if (strcmp(message,"HUMIDITY")==0) {
      check_point++;
       count=0;
       float humidity=0.0;
       while(payload[check_point]!=' ' && check_point<payload_size) {message[count]=payload[check_point]; check_point++; count++;}
       message[count]='\0';
       humidity=atof(message);
       sensor_values.humidity=humidity;
       sensor_values.temperature=0.0;
    }
    else if (strcmp(message,"TEMPERATURE_AND_HUMIDITY")==0) {
       check_point++;
       count=0;
       float humidity=0.0;
       float temperature=0.0;
       while(payload[check_point]!='&' && check_point<payload_size) {message[count]=payload[check_point]; check_point++; count++;}
       message[count]='\0';
       temperature=atof(message);
       check_point++;
       count=0;
       while(payload[check_point]!=' '  && check_point<payload_size) {message[count]=payload[check_point]; check_point++; count++;}
       message[count]='\0';
       humidity=atof(message);
       sensor_values.humidity=humidity;
       sensor_values.temperature=temperature;

    } 
    else {printf("The arduino did not answer hello correctly\n");return EXE_ERR;}
    check_point++;
    int size_end=sizeof(ended)+check_point-1;
    count=0;
    while(check_point<size_end)
     {   
        if(ended[count]!=payload[check_point])  {printf("The arduino did not answer hello correctly\n");return EXE_ERR;}
        check_point++;
        count++;
     }
     StringDate(sensor_values.date,sensor_values.time);
     return EXE_OK;
  }
  printf("The arduino did not answer hello correctly\n");return EXE_ERR;
 }
  
 if(strcmp(message,"FAIL")==0) { printf("the packet sent was not correctly received in the arduino\n");return EXE_ERR;}
 printf("the information received does not have sense\n");
 return EXE_ERR;
}


void Serial_communication::StringDate(char *date,char *time){
  
  time_t t;
  struct tm * information;
  std::time(&t);
  information= localtime(&t);

  char day[5];
  char month[5];
  char year[5];
  char hour[5];
  char minutes[5];
  char seconds[5];

  if (information->tm_mday < 10)sprintf(day,"%02d", (int)information->tm_mday);
  else sprintf(day,"%d", (int)information->tm_mday);
  if ((information->tm_mon + 1) < 10)sprintf(month,"%02d", (int)information->tm_mon + 1);
  else sprintf(month,"%d", (int)information->tm_mon + 1);

  sprintf(year,"%d", information->tm_year + 1900);
  
  sprintf(date,"%s-%s-%s",year,month,day);

  if (information->tm_hour < 10) sprintf(hour,"%02d", (int)information->tm_hour);
  else sprintf(hour,"%d", (int)information->tm_hour);
  
  if (information->tm_min < 10) sprintf(minutes,"%02d", (int)information->tm_min);
  else sprintf(minutes,"%d", (int)information->tm_min);
  
  if (information->tm_sec < 10) sprintf(seconds,"%02d", (int)information->tm_sec);
  else sprintf(seconds,"%d", (int)information->tm_sec);
  sprintf(time,"%s:%s:%s",hour,minutes,seconds);
}
  


