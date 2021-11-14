#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef LINUX_KEY_WORD
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#elif WINDOWS_KEY_WORD
    #include <WinSock2.h>
#else 
  #error "OS not supported!"
#endif

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
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
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    printf("[+] Connecting to the server...\n");
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("[+] Connection Granted!\n");
    while(1){

        bzero(client_message,256);
        bzero(server_message,256);

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
