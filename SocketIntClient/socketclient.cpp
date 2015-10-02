#include "socketclient.h"
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr.in and inet.addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close */
#include <sys/types.h>
#include <signal.h>

#define RCVBUFSIZE 32 /*Size ofreceive buff */


SocketClient::SocketClient(int Port, string ip)
{
    int sock;
    struct sockaddr_in echoServAddr;
    unsigned short echoServPort = Port; //Make sure we are using the server port here!
    char *servIP;
    char  echoString[RCVBUFSIZE];
    char echoBuffer[RCVBUFSIZE];
    unsigned int echoStringLen;
    int bytesRcvd, totalBytesRcvd;

    //servIP = "127.0.0.1"; //TODO make these come from form
    servIP = (char*) ip.c_str();
    strncpy(echoString, "Hello", sizeof(echoString));

    //Create a new process so we don't process block, buttons only make threads
    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
      printf("Socket failed to create! \n");
      return;
    }

    printf("Created socket. \n");
    printf("Server port: %i \n", Port); 
    fflush(stdout);
    
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port = htons(echoServPort);

    if(connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0){
      printf("connect failed! \n");
      fflush(stdout);
      return;
    }

    while(strcmp(echoString, "Close") != 0){ //TODO add "die"
      echoStringLen = strlen(echoString);
      if(send(sock, echoString, echoStringLen, 0) != echoStringLen){
        printf("Send failed! Different number of bytes then expected \n");
        return;
      }

      if (strcmp(echoString, "ls") == 0){
        ListDirectory(sock); //List Directory
      }
      else{
        totalBytesRcvd = 0;
	printf("Recieved: ");
	while (totalBytesRcvd < echoStringLen){
	  if((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) < 0){
            printf("Connection closed early or recv() failure!");
	    return;
	  }
	
        totalBytesRcvd += bytesRcvd;
	echoBuffer[bytesRcvd] = '\0';
	printf(echoBuffer);
	}
		
	printf("\n");
	fflush(stdout);
      }
      
      printf("Command: ");
      scanf("%s[^n]",echoString);
    }
    close(sock);
}


void SocketClient::ListDirectory(int sock)
{
    long Input;
    char Directory[32];
    long totalBytesRcvd = 0;
    long bytesRcvd = 0;
	
    while (totalBytesRcvd < sizeof(long)){ //Get a long
      if((bytesRcvd = recv(sock, &Input, RCVBUFSIZE, 0)) < 0){
        printf("Connection closed early or recv() failure!");
     	return;
      }

      totalBytesRcvd += bytesRcvd;
      printf("File Size: %ld\n",Input);
      fflush(stdout);
		}
      if(send(sock, "ok", 2, 0) != 2){ //Send that we got the length of file.
        printf("Send failed! Different number of bytes then expected \n");
        return;
      }

      printf("Sent ok \n");
      fflush(stdout);
      totalBytesRcvd = 0;
      bytesRcvd = 0;

      while (totalBytesRcvd < Input){ //Get a long
        if((bytesRcvd = recv(sock, Directory, RCVBUFSIZE-1, 0)) < 0){
	  printf("Connection closed early can not get Directory!");
     	  return;
    	}
     	totalBytesRcvd += bytesRcvd;
	Directory[bytesRcvd] = '\0';
	printf("%s", Directory);
	fflush(stdout);
	memset(Directory, 0, sizeof(Directory));

    }

    printf ("\n");
    printf ("%ld", totalBytesRcvd);	
    fflush(stdout);
}
