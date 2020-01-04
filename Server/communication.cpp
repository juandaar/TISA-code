#include "communication.h"

communication::communication(){
	if(socket_create()==EXE_OK){
		if(server_address(SERVER_PORT)==EXE_OK)printf("Client communication was correctly created\n");
		else printf("Client communication was not correctly created\n");

	}
	else printf("Client communication was not correctly created\n");
}

exception communication::socket_create(){
	if( (local_socket= socket(FAMILY_SOCKET,TYPE_SOCKET,PROTOCOL_SOCKET)) < 0){
		puts("socket construction problem"); 
		exit(1);  
	}
	return EXE_OK;
}


exception communication::server_address(int server_port){
  
    memset((char *) &server, 0, sizeof(server));  
	server.sin_family = AF_INET;				   	 
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(server_port);
    bind(local_socket, (struct sockaddr *) &server, sizeof(server));
	return EXE_OK;
}






exception communication::send(char *message) {
 if (sendto(local_socket,message,strlen(message)+1,0,(struct sockaddr *)&client,sizeof(client))<0) {
   printf("the send process has failed\n");
   return EXE_ERR;
 }
  return EXE_OK;

}





int  communication::receive(char *message, int message_buffer) {
    struct sockaddr_in client_temp;
    int receive_size=0;
	memset((char *)&client_temp, 0, sizeof(struct sockaddr_in));
	client_temp.sin_family = AF_INET;
	client_size = sizeof(struct sockaddr_in);
	receive_size = recvfrom(local_socket,message,message_buffer, 0,(struct sockaddr *)&client_temp,&client_size);
  //  RecievedBytes = recvfrom(LocalSocket, Buffer, BufferSize, 0, (struct sockaddr *) AddressClient, ClientSize);
	if(receive_size < 0)
	{
		printf("the receive process has failed\n");
		return -1;
	}
	 if(client_temp.sin_port!=client.sin_port)client=client_temp;
	return receive_size;

}


int communication::receive(gbnp *packet,int lenght){
	struct sockaddr_in client_temp;
    int receive_size=0;
	memset((char *)&client_temp, 0, sizeof(struct sockaddr_in));
	client_temp.sin_family = AF_INET;
	client_size = sizeof(struct sockaddr_in);
	receive_size = recvfrom(local_socket,packet,lenght, 0,(struct sockaddr *)&client_temp,&client_size);
  //  RecievedBytes = recvfrom(LocalSocket, Buffer, BufferSize, 0, (struct sockaddr *) AddressClient, ClientSize);
	if(receive_size < 0)
	{
		printf("the receive process has failed\n");
		return -1;
	}
    if(client_temp.sin_port!=client.sin_port)client=client_temp;
	return receive_size;
}


exception communication::maybe_send(gbnp packet){
    
    //packet no lost 
 	if(rand()>LOSS_PROBABILITY*RAND_MAX) {
 		//packet no corrupt
 		if(rand()>CORRUPTION_PROBABILITIY*RAND_MAX){
               const void *message=&packet;
               size_t length=sizeof(packet);
 			 if (sendto(local_socket,message,length+1,0,(struct sockaddr *)&client,sizeof(client))>0) return EXE_OK;
 		
 			 else {printf("error\n");return EXE_ERR;}

 		}
 		else {
 			   packet.data[0]='a';
 			   packet.data[1]='b';
 			   packet.data[2]='\n';
               const void * message=&packet;
               size_t length=sizeof(packet);
 			if (sendto(local_socket,message,length+1,0,(struct sockaddr *)&client,sizeof(client))>0) return EXE_OK;
 			else return EXE_ERR;

 		}

 	}
 	else  {
    //packet lost
 		printf("The packet was not send\n");
 		return EXE_OK;

 	}

}