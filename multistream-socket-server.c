/*
* Name:          Multistream Socket Server (in C)
* Created:       20160629-12:51-GMT+1
*
* Creator:       Kim Olsen <post@kimeriktjoraolsen.no>
* Lastchange:   20170320-13:48-GMT+1
*
* Multistream socket server, implemented in ANSI C.
*/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

void doprocessing (int sock) {
   int n;
   char buffer[256]; /* 256 bytes character buffer */
   bzero(buffer,256);
   n = read(sock,buffer,255); /* Read input (up to 255 chars) from socket */   
   if (n < 0) { /* Error on input from socket */
      perror("MSS: ERROR reading from socket\n");
      exit(1); /* Error-exit 1 */
   }
   
   printf("Here is the message: %s\n",buffer);
   n = write(sock,"I got your message",18); /* Write 18-Byte output to socket */ 
   if (n < 0) { /* Error on output to socket */
      perror("MSS: ERROR writing to socket\n");
      exit(1); /* Error-exit 1 */
   }
}

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[256]; /* 256 bytes character buffer */
   struct sockaddr_in serv_addr, cli_addr;
   int n, pid;
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
      perror("MSS: ERROR opening socket\n");
      exit(1); /* Error-exit 1 */
   }
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5001; /* Port number to use for "multistream-socket-server" instance */
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("MSS: ERROR on binding\n");
      exit(1); /* Error-exit 1 */
   }
   
   /* Now start listening for the clients, here
    * process will go in sleep mode and will wait
    * for the incoming connection(s).
    */
   listen(sockfd,5);
   clilen = sizeof(cli_addr); /* Client length equals sizeof (incoming) client-address */
   while (1) {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);		
      if (newsockfd < 0) {
         perror("MSS: ERROR on accept\n");
         exit(1); /* Error client-exit 1 */
      }
      /* Create child process (fork) */
      pid = fork();	
      if (pid < 0) {
         perror("MSS: ERROR on fork\n");
         exit(1); /* Error client-exit 1 */
      }
      if (pid == 0) {
         /* This is the client process */
         close(sockfd);
         doprocessing(newsockfd);
         exit(0); /* Successful client-exit 0 */
      }
      else {
         close(newsockfd); /* Close new connection */
      }	
   } /* end of while */
}
