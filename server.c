/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#elif __WIN32
    #include <winsock2.h>
   
#else 

#endif

// A function to print errors
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
	#ifdef __linux__
    	int sockfd, newsockfd, portno;
    	int clilen;
    	struct sockaddr_in serv_addr, cli_addr; // Two structure to hold server socketoptions and client socket option such as IP and Port
    #elif __WIN32
    	WSADATA wsaData;
    	SOCKET sockfd;
    	SOCKET newsockfd;
    	SOCKADDR_IN serv_addr, cli_addr;
    	int portno, clilen;

		if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
		{
        	printf("Server: WSAStartup failed with error %ld\n", WSAGetLastError());
        	// Exit with error
			return -1;
    	}
    	else
		{
        	printf("Server: The Winsock dll found!\n");
       	 	printf("Server: The current status is %s.\n", wsaData.szSystemStatus);
    	}
    #else
    #endif
    
    char client_message[256];
    char server_message[256];

    // Check whether the port value is provided by the user or not
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    // create a server TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd  == INVALID_SOCKET) 
       error("ERROR opening socket");

    //clean the memory buffer of serv_addr
    //bzero((char *) &serv_addr, sizeof(serv_addr));
    memset(&serv_addr, 0, sizeof(serv_addr));

    // Turn the privided port by user into an integer
    portno = atoi(argv[1]); 
    
    // Set server IP and port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(portno); // Turn the port from host format to network format

    // Bind the socket with IP and port
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
             error("ERROR on binding");

    // Listen for client connection MAX 5 connections
    printf("[+] Listening to incoming connections...\n");
    listen(sockfd,5);

    clilen = sizeof(cli_addr);

    // Accept an incoming connection from the client
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) // check whether the socket is opened or not 
         error("ERROR on accept");
         
    printf("[+] Connection accepted...\n");
    printf("==================== Starting the chat ===================\n");

    while(1){
        // Clean the buffers
        //bzero(client_message,256);
        memset(&client_message, 0, sizeof(client_message));
        //bzero(server_message,256);
        memset(&server_message, 0, sizeof(server_message));

        // Reveive message from the client    
        if(recv(newsockfd,client_message,sizeof(client_message), 0) < 0)
           error("ERROR reading from socket");

        printf("Client: %s\n",client_message);

        printf("Server: ");
        fgets(server_message, 256, stdin);

        // send server message to the client
        if(send(newsockfd,server_message,strlen(server_message), 0) < 0)
            error("ERROR writing to socket");
        
        if (strncmp("exit", client_message, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }

    // close both client and server sockets
    close(newsockfd);
    close(sockfd);
    return 0; 
}
