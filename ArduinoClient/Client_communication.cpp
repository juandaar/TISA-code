#include "Client_communication.h"

Client_communication::Client_communication(){
	if(socket_create()==EXE_OK){
		if(server_address(SERVER_HOST,SERVER_PORT)==EXE_OK)printf("Client communication was correctly created\n");
		else printf("Client communication was not correctly created\n");

	}
	else printf("Client communication was not correctly created\n");
}

exception Client_communication::socket_create(){
	if( (local_socket= socket(FAMILY_SOCKET,TYPE_SOCKET,PROTOCOL_SOCKET)) < 0){
		puts("socket construction problem"); 
		exit(1);  
	}
	return EXE_OK;
}


exception Client_communication::server_address(char*server_addr,int server_port){	
     
 struct hostent *internet_destination; //destination address in proper format
 struct in_addr destination_ip; //destination address in numeric ip format
 if(inet_aton(server_addr,&destination_ip)) internet_destination= gethostbyaddr((char*)&destination_ip,sizeof(destination_ip),AF_INET);
 else internet_destination=gethostbyname(server_addr);
 if(internet_destination==NULL) {
 	fprintf(stderr,"invalid network address\n");
 	exit(1);
 
 }
 memset((char*)&server,0,sizeof(server));
 memcpy(&server.sin_addr,internet_destination->h_addr_list[0],sizeof(server.sin_addr));
 server.sin_family=AF_INET;
 server.sin_port=htons(server_port);
 return EXE_OK;
}


exception Client_communication::send(char *message) {
 if (sendto(local_socket,message,strlen(message)+1,0,(struct sockaddr *)&server,sizeof(server))<0) {
   printf("the send process has failed\n");
   return EXE_ERR;
 }

  return EXE_OK;

}




int  Client_communication::receive(char *message, int message_buffer) {

	int receive_size=0; 
	memset((char *)&client, 0, sizeof(struct sockaddr_in));
	client.sin_family = AF_INET;
	client_size = sizeof(struct sockaddr_in);
	receive_size = recvfrom(local_socket,message,message_buffer, 0,(struct sockaddr *)&client,&client_size);
  //  RecievedBytes = recvfrom(LocalSocket, Buffer, BufferSize, 0, (struct sockaddr *) AddressClient, ClientSize);
	if(receive_size < 0)
	{
		printf("the receive process has failed\n");
		return 0;
	}
	
	return receive_size;

}




exception Client_communication::maybe_send(gbnp packet){
    
    //packet no lost 
 	if(rand()>LOSS_PROBABILITY*RAND_MAX) {
 		//packet no corrupt
 		if(rand()>CORRUPTION_PROBABILITIY*RAND_MAX){
               const void *message=&packet;
               size_t length=sizeof(packet);
 			 if (sendto(local_socket,message,length+1,0,(struct sockaddr *)&server,sizeof(server))>0) return EXE_OK;
 			 else return EXE_ERR;

 		}
 		else {
 			   packet.data[3]='a';
 			   packet.data[4]='a';
               const void * message=&packet;
               size_t length=sizeof(packet);
 			if (sendto(local_socket,message,length+1,0,(struct sockaddr *)&server,sizeof(server))>0) return EXE_OK;
 			else return EXE_ERR;

 		}

 	}
 	else  {
    //packet lost
 		printf("The packet was not send\n");
 		return EXE_OK;

 	}

}


int Client_communication::receive(gbnp *packet,int lenght){
	
    int receive_size=0;
    
	memset((char *)&client, 0, sizeof(struct sockaddr_in));
	client.sin_family = AF_INET;
	client_size = sizeof(struct sockaddr_in);
	receive_size = recvfrom(local_socket,packet,lenght, 0,(struct sockaddr *)&client,&client_size);
  //  RecievedBytes = recvfrom(LocalSocket, Buffer, BufferSize, 0, (struct sockaddr *) AddressClient, ClientSize);
	if(receive_size < 0)
	{
		printf("the receive process has failed\n");
		return -1;
	}
	
	return receive_size;
}



