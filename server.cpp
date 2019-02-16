/*********************************************************************************************************************
  Owner Name: SHASHANK KAMATH KALASA MOHANDAS	
  UIN: 627003580
  Program functionality: SERVER program
 *********************************************************************************************************************/

#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>

#define BACKLOG 10
#define MAXDATASIZE 1024 
/*********************************************************************************************************************
The getaddress is a user defined function which compares the family to IPv4 or IPv6 and returns the IP address.
*********************************************************************************************************************/
void *getaddress(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) 
	{
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
		return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*********************************************************************************************************************
The getport is a user defined function which compares the family to IPv4 or IPv6 and returns the port number.
*********************************************************************************************************************/
void *getport(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) 
	{
        return &(((struct sockaddr_in*)sa)->sin_port);
    }
		return &(((struct sockaddr_in6*)sa)->sin6_port);
}

int main(int argc, char *argv[])
{
	char *PORT;
	char s[INET6_ADDRSTRLEN];
	char message[MAXDATASIZE]={0}; //Creating a buffer to hold the data received
	
	int sfd, newsfd, flag, numbytes, yes=1;

	struct addrinfo addressinfo, *servicelist, *loopvariable;
	struct sockaddr_storage str_addr;
	socklen_t addr_size;
	
	pid_t childpid; // Creating a variable to hold child process PID
	
	PORT = argv[1];
	
	memset(&addressinfo,0,sizeof (addressinfo)); // Making the addressinfo struct zero
	addressinfo.ai_family = AF_UNSPEC;// Not defining whether the connection is IPv4 or IPv6
	addressinfo.ai_socktype = SOCK_STREAM;
	addressinfo.ai_flags = AI_PASSIVE;
	
	if ((flag = getaddrinfo(NULL, PORT, &addressinfo, &servicelist)) != 0) 
	{
		printf("GetAddrInfo Error");
		exit(1);
	}
	printf("Server: Done with getaddrinfo \n");
	// Traversing the linked list for creating the socket
	for(loopvariable = servicelist; loopvariable != NULL; loopvariable = (loopvariable -> ai_next ))
	{
		if((sfd = socket(loopvariable -> ai_family, loopvariable -> ai_socktype, loopvariable -> ai_protocol)) ==  -1 )
		{
			printf("Server: Socket Created.\n");
			continue;
		}
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==  -1) 
		{
			perror("Server: SetSockOpt.\n");
			exit(1);
		}
		//Binding the socket
		if (bind(sfd, loopvariable->ai_addr, loopvariable->ai_addrlen) ==  -1)	
		{
			close(sfd);
			perror("Server: Bind Error.\n");
			continue;
		}
		break;
	}
	// Freeing the linked  list
	freeaddrinfo(servicelist); 

	if (loopvariable == NULL) 
	{
		printf("Server: Failed to bind.\n");
		exit(1);
	}
	
	// Listen to the connection
	if (listen(sfd, BACKLOG) ==  -1) 
	{
		perror("Server: Listen Error");
		exit(0);
	}
	printf("Server: Listening in progress \n");
	
	//Accept the connection	
	while(1)
	{
		addr_size = sizeof str_addr;
		newsfd = accept(sfd, (struct sockaddr *)&str_addr, &addr_size);
			if (newsfd == -1)
			{
				perror("Server: Accept Error");
				exit(1);
			}
		printf("Server: Connection Accepted \n");
		
		inet_ntop(str_addr.ss_family,  getaddress((struct sockaddr *)&str_addr), s, sizeof s);
		printf("Server: Connection from %s\n", s);
		//printf("Server: Connection port number: %d \n", ntohs(getport((struct sockaddr *)&str_addr)));
		//Creating a child process for multiple connections
		if((childpid = fork()) == 0)
		{
			printf("Server: Child Process created with PID: %d \n", getpid());
			close(sfd);	// Closing the old socket
			while(1)
			{
				if ((numbytes = recv(newsfd, message, MAXDATASIZE-1, 0)) == -1) // Receving the data from client 
				{
					printf("Server: Receiving Error");
					exit(1);
				}
					message[numbytes] = '\0';
					printf("Server: Received : '%s'  \n", message);
					if(strcmp(message,"exit") == 0) // if message is "exit" then close the new socket 
					{
						close(newsfd);
						printf("Server: Client Disconnected");
						break;
					}
				
					if(send(newsfd, message, strlen(message), 0) ==  -1) // Sending the data back to client
					{
						perror("Server: Send Error");
						exit(1);
					}
					printf("Server: Echoing:  '%s' \n", message);
					bzero(message, sizeof(message));
			}
		}
	}
	close(newsfd);	// Closing the new socket
	return 0;
}
