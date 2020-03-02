#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
   int seq_no;
   char buffer;
} frame;

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
     int sockfd, newsockfd, portno, clilen;
     char* msg = "pikachu";
     int length = 7;

     frame* s = (frame*) malloc(sizeof(frame));
     int count = 0;

     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
     //while(1) {
     for(int i=0;i<length;++i) {
	s -> buffer = msg[i];
	s -> seq_no = i%2;

    	//n = read(newsockfd,buffer,255);
     	//if (n < 0) error("ERROR reading from socket");
     	//printf("%s\n",buffer);
     	//bzero(buffer, 256);
     	//fgets(buffer, 255, stdin);
     	
	n = write(newsockfd,s,sizeof(frame));
     	if (n < 0) error("ERROR writing to socket");
	
	// waiting for acknowledgement
	frame* ack = (frame*) malloc(sizeof(frame));
	n = read(newsockfd, ack, sizeof(frame));
	printf("Ack number: %d\n", ack -> seq_no);
     }
     return 0;
}
