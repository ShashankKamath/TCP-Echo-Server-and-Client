/*********************************************************************************************************
  Owner Name: Srinivas Prahalad Sumuka
  UIN: 627008254
  Program functionality: client program
 *********************************************************************************************************/


#include<unistd.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<stdio.h>
#include<netdb.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
//#include<unp.h>

#define debug 0
#define String_size 1024
/**********************************************************************************************************

  The writen is a user defined function which uses built in function such as send to send the input message
  to the server. If there is any error while sending the message, function send returns a negitive value 
  which implies it an error. If there isn't any error then the function send returns a value 0. The function
  writen returns 0 or 1 based on the above explained condition.

 ***********************************************************************************************************/
int writen(int socketfd, char *msg, int str_length, int flag) {
#if debug
	//msg = "Hello world";
	//int string_len= strlen("I am srinivas");
	//int sendto_ack = sendto(socketfd, msg, string_len, 0, server_ptr->ai_addr, server_ptr->ai_addrlen);
#endif
	int sendto_ack = send(socketfd, msg, str_length, flag); //checking for any error while sending
	printf("Client Sending: %s \n",msg);
	return sendto_ack;
	//return 0;
}
/***********************************************************************************************************

  readline is a user defined function which is used to read the data sent by the server. readline uses the 
  built in function recv to read the data. The function returns 0 if read is completed otherwise it will
  send negitive value indicating error

 ************************************************************************************************************/
int readline(int socketfd, char buffer[], int receiving_size, int flag) {
	int receive_from_ack = recv(socketfd, buffer, receiving_size, flag);
	if(receive_from_ack < 0)
	{
#if debug
		printf("receiving error");
#endif		
		perror("error while receiving");
	}
	buffer[receive_from_ack] = '\0';
#if debug
	printf("\nClient Received: %s \n",buffer);
	printf("\n at the end\n");
#endif
	printf("\nClient Received:");
	fputs(buffer,stdout);
	return receive_from_ack;
}

int main(int argc, char *argv[]) {
	int status,flag=0;
	int string_len;
	char buffer[String_size];
	int receive_from_ack;
	int receiving_size = sizeof (buffer);
	socklen_t *fromlength;
	addrinfo server, *server_ptr;
	void *address_ptr_void;
	char *address,*port;
	char ipstr[INET6_ADDRSTRLEN];
	char input_string[String_size];
	char *msg; 
	int socketfd;
	if (argc != 3)
	{

#if debug
		printf("\nEnter Ip and port\n");

#endif
		exit(0);
	}
	address = argv[1];
	port = argv[2];
	server.ai_family = AF_UNSPEC; //can be AF_INET
	server.ai_socktype = SOCK_STREAM; 
	server.ai_flags = AI_PASSIVE;
	status = getaddrinfo(address, port, &server, &server_ptr);
	//printf("status = %d \n",status);
	if(status !=0) {
#if debug
		printf("\nunable to connect to ip address %s\n",address);
#endif
		exit(0);
	}
	addrinfo *ptr_copy;
	ptr_copy = server_ptr;

	for(ptr_copy = server_ptr; ptr_copy != NULL; ptr_copy= ptr_copy->ai_next) {    //going through the list
		struct sockaddr_in *ptr_with_address = (struct sockaddr_in *) ptr_copy->ai_addr;
		address_ptr_void = &(ptr_with_address->sin_addr);
	}
	char *ipver = "IPv4";
	inet_ntop(server.ai_family, address_ptr_void, ipstr,sizeof ipstr);
	//printf("\n address is %s",ipstr);

	for(ptr_copy = server_ptr; ptr_copy != NULL; ptr_copy= ptr_copy->ai_next) {
		struct sockaddr_in *ptr_with_address = (struct sockaddr_in *) ptr_copy->ai_addr;
		address_ptr_void = &(ptr_with_address->sin_addr);
	}
	socketfd = socket(server_ptr->ai_family, server_ptr->ai_socktype, server_ptr->ai_protocol);
	if (socketfd == -1) {
#if debug
		printf("\nError at the socket call\n");
#endif
		perror("Error at socket");
		exit(0);
	}
	int connect_ack = connect(socketfd,server_ptr->ai_addr, server_ptr->ai_addrlen); //returns negitive value on failure to connect
	if(connect_ack == -1) {
#if debug
		printf("\ncould not connect\n");
#endif
		perror("Problem with connect");
		exit(0);
	}
	while(1)
	{
		printf("\nEnter the string\n");
		char * check_end = fgets(input_string,100,stdin);
		if(check_end == NULL) //send exit if ctrl D is pressed
		{
			msg = "exit";
			int sending_data = writen(socketfd, msg, string_len, flag);
			exit(0);
		}
		msg = &input_string[0]; // user input to message pointer
#if debug
		//strtok(msg, "\n");
#endif
		strtok(msg, "\n");
		string_len= strlen(input_string);	
		int sending_data = writen(socketfd, msg, string_len, flag); // function call to write
		if(sending_data == -1) 
		{
			printf("\nerror sending\n");
		} 
		int receiving_data = readline(socketfd,buffer,receiving_size, flag); // function call to read
		if(receiving_data == -1) {
			printf("\nerror sending\n");
		} 
#if debug		
		else {
			printf("sending data...\n");
		}
#endif
	}
	close(socketfd);
	freeaddrinfo(server_ptr);
	return 0;
}
