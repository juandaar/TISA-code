
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

//builder implementation
Serial_communication::Serial_communication(Stream &stream,DHT11 *dht11) : m_stream(stream) { 
  wdt_disable(); //disable watchdog
  m_dht11=dht11;
}

//loop implementation,does not return anything
void Serial_communication::loop(){
  read(); //read is blocked
  send(); //if it listens something that it can understand, it answers it. If it listens something that it can not understand, it answers a fail message
}


//read implementation, return a exception value
exception Serial_communication::read(){
  String message="";
  while(!m_stream.available()); //wait
  while(m_stream.available())message+=m_stream.readString(); //reads a string value received to the Serial port
  delay(20);
  if(message.length()<=BUFFER_SIZE)  //checks if the information is more length that the BUFFER_SIZE limit
     if(read_package(message)==EXE_OK) return EXE_OK; //checks if the package received is understoond
     
  return EXE_ERR;
}

//read package, return a exception value
exception Serial_communication::read_package(String message){
 next_package=FAIL; //all packet received is wrong if it does not demonstrate that is false
 int message_size=message.length()+1; //message size
 char message_char[message_size]; //char message vector
 message.toCharArray(message_char,message_size); //convert from string to char vector
 int check_point=0; //message vector space
 char init[]=INIT_PACKAGE;
 char ended[]=END_PACKAGE;
 //check if the package init with INIT_PACKAGE string
 while(check_point<sizeof(init)-1){
  if(init[check_point]!=message_char[check_point]) return EXE_ERR;
  check_point++;
 }
 check_point++;
 int count=0;
 message="";
 //checks the communication_package type
 while(message_char[check_point]!=' ' &&  count<5 && check_point<message_size){message+=message_char[check_point]; check_point++; count++;}
 if(message=="GET") package=GET; else if(message=="POST") package=POST; else if (message=="HELLO")package=HELLO; else return EXE_ERR;
 //GET PACKAGE
 if(package==GET) {
  check_point++;
  count=0;
  message="";
  int information_size=0;
  //count the payload information length
  while(message_char[check_point]!=' ' && count<2 && check_point<message_size) {message+=message_char[check_point]; check_point++; count++;}
  information_size=message.toInt()-2;
  
  if(information_size<=MAXIMUM_SENSOR_VALUE_STRING and information_size>0) {
    check_point++;
    count=0;
    message="";
    while(message_char[check_point]!=' ' && count<information_size && check_point<message_size) {message+=message_char[check_point]; check_point++; count++;}
    if(count!=information_size) return EXE_ERR;
    //checks the sensor type that the user needing
    if(message=="TEMPERATURE")sensor_kind=TEMPERATURE; 
    else if (message=="HUMIDITY") sensor_kind=HUMIDITY; 
    else if (message=="TEMPERATURE_AND_HUMIDITY") sensor_kind=TEMPERATURE_AND_HUMIDITY;
    else  return EXE_ERR;
    check_point++;
    int size_end=sizeof(ended)+check_point-1;
    count=0;
    while(check_point<size_end)
     {
        if(ended[count]!=message_char[check_point]) return EXE_ERR;
        check_point++;
        count++;
     }
     if(sensor_value()!=EXE_OK) return EXE_ERR; //the sensor values was not correctly collected
     next_package=OK;
     return EXE_OK; //the GET petition was correctly undestanded and collected
  } 
  return EXE_ERR;
 }
  //HELLO PACKAGE
  if(package==HELLO) {
    check_point++;
    int size_end=sizeof(ended)+check_point-1;
    while(check_point<size_end)
     {
        if(!init[check_point]==message_char[check_point]) return EXE_ERR;//the hello package is wrong
        check_point++;
     }
    next_package=HI; 
    return EXE_OK; //the hello package is well
  }
 return EXE_ERR;
}

//send implementation, return a exception
exception Serial_communication::send(){
  String message=INIT_PACKAGE;
  //send creates the answer messages
  if(next_package==OK) { 
    message+=" OK ";
    int lg=last_sensor_value.length();
    message+=lg;message+=" ";message+=last_sensor_value;message+=END_PACKAGE; 
  }
  else if(next_package== FAIL) {
    message+=" FAIL ";
    message +=END_PACKAGE;
  }
    else if(next_package== HI) {
    message+=" OK HELLO ";
    message +=END_PACKAGE;
  }
  else return EXE_ERR; 
  message+="\n";
  m_stream.print(message);
  return EXE_OK; //ok
}
//sensor_value implementation, return exception
exception Serial_communication::sensor_value(){
  last_sensor_value="";
  if(sensor_kind==TEMPERATURE || sensor_kind==HUMIDITY || sensor_kind==TEMPERATURE_AND_HUMIDITY) {
    float temperature,humidity;
    if((m_dht11->read(humidity, temperature))==0){
      //the sensor_value collects the  sensor values and it creates the string information structure
      if (sensor_kind==TEMPERATURE) {last_sensor_value+="TEMPERATURE ";last_sensor_value+=temperature;last_sensor_value+=" ";}
      else if(sensor_kind==HUMIDITY){ last_sensor_value+="HUMIDITY ";last_sensor_value+=humidity;last_sensor_value+=" ";}
      else if(sensor_kind==TEMPERATURE_AND_HUMIDITY){ last_sensor_value+="TEMPERATURE_AND_HUMIDITY ";last_sensor_value+=temperature; last_sensor_value+="&";last_sensor_value+=humidity;last_sensor_value+=" ";}
      return EXE_OK;
    }
    else return EXE_ERR; 
  }
  return EXE_ERR;
}

