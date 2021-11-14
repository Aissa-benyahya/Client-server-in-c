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
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	#ifdef __linux__
    	int sockfd, portno;
    	struct sockaddr_in serv_addr; // Two structure to hold server socketoptions and client socket option such as IP and Port
    #elif __WIN32
    	WSADATA wsaData;
    	SOCKET sockfd;
    	SOCKADDR_IN serv_addr;
    	int portno;

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
    struct hostent *server;

    char client_message[256], server_message[256];

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    // get the IP using the name like (www.google.com)
    #ifdef __linux__
    	server = gethostbyname(argv[1]);
    	if (server == NULL) {
        	fprintf(stderr,"ERROR, no such host\n");
        	exit(0);
    	}
	#endif
		
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    #ifdef __WIN32
    	if (isalpha(argv[1][0]))
    	{   // server address is a name
        	server = gethostbyname(argv[1]);
    	}
    	else
    	{ // Convert nnn.nnn address to a usable one
        	int addr = inet_addr(argv[1]);
        	server = gethostbyaddr((char *)&addr, 4, AF_INET);
    	}
    #endif
    memcpy(&(serv_addr.sin_addr), server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    printf("[+] Connecting to the server...\n");
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("[+] Connection Granted!\n");
    while(1){

        memset(client_message,0, strlen(client_message));
        memset(server_message,0, strlen(server_message));

        printf("Client: ");
        fgets(client_message,255,stdin);
        if(send(sockfd,client_message,strlen(client_message), 0) < 0)
             error("ERROR sending to socket");

        if(recv(sockfd,server_message,sizeof(server_message), 0) < 0) 
             error("ERROR reading from socket");

        printf("Server: %s\n",server_message);

        if (strncmp("exit", server_message, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
