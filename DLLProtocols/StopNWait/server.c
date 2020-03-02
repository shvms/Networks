#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

typedef struct {
   int seq_no;
   int ack_no;
   int buffer;
} frame;

void error(char *msg) {
    perror(msg);
    exit(1);
}

int add_error(char c) {
    int no_of_ones, i;
    int bin[8];
    int n;

    for (i = 0; i < 8; i++) {
       bin[i] = !!((c << i) & 0x80);
       if (bin[i] == 1) { no_of_ones++; }
    }

    // if (no_of_ones%2 != 0) { n += 1; }

    for(int i=1;i<8;++i) { bin[i-1] = bin[i]; }
    bin[7] = no_of_ones%2 == 0 ? 0 : 1;

    // introducing error
    bin[4] = bin[4] == 0 ? 1 : 0;

    n = 0;
    for(int i=7;i>=0;--i) {
        n += pow(2, i) * bin[i];
    }

    return n;
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
	s -> buffer = add_error(msg[i]);
	s -> seq_no = i%2;
	printf("Sending message: %d\n", s->buffer);

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
	printf("Ack number: %d\n", ack -> ack_no);
     }
     return 0;
}
