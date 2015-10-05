#include "socketclient.h"
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr.in and inet.addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close */
#include <sys/types.h>
#include <signal.h>
#include <fstream>
#include <iostream>

#define RCVBUFSIZE 32 /*Size ofreceive buff */


SocketClient::SocketClient(int Port, string ip)
{
    int sock;
    struct sockaddr_in echoServAddr;
    unsigned short echoServPort = Port; //Make sure we are using the server port here!
    char *servIP;
    char echoString[RCVBUFSIZE];
    char echoBuffer[RCVBUFSIZE];
    unsigned int echoStringLen;
    int bytesRcvd, totalBytesRcvd;

    //servIP = "127.0.0.1"; //set serv IP. and initial message.
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
      if(send(sock, echoString, echoStringLen, 0) != (int) echoStringLen){
        printf("Send failed! Different number of bytes then expected \n");
        return;
      }

      /*ask to list the directory*/
      if (strcmp(echoString, "ls") == 0){
        ListDirectory(sock); //List Directory
      }
      
      /*if asking for a file!*/
      else if(strcmp(echoString, "get") == 0){
	
	memset(echoBuffer,0,RCVBUFSIZE);
        if (!feof(stdin));
		scanf("%*c");
	scanf("%[^\n]s",echoBuffer);
        	
	echoStringLen = strlen(echoString);
        if(send(sock, echoBuffer, RCVBUFSIZE-1, 0) != RCVBUFSIZE-1){
        	printf("Send failed! Different number of bytes then expected \n");
        	return;
      	}
	long i = 0;
	if((bytesRcvd = recv(sock, &i, RCVBUFSIZE, 0)) < 0){
		    printf("Connection closed early or recv() failure!");
		    return;
	}
	
	if (i > 0){ //If file exists, we can transfer it.
		if((recvFile(i, echoBuffer, sock)) != 0){//Get file
			printf("File failed to transfer");
		} 
		else{
			printf("Transfer complete");
		}
	}
	else{
		printf("file does not exist.\n");//It does not exist.
	}
	memset(echoBuffer,0,RCVBUFSIZE);
      }
	
      else if(strcmp(echoString, "cd") == 0){
	printf("To what directory?: ");
        scanf("%s",echoString);
	echoStringLen = strlen(echoString);
        if(send(sock, echoString, echoStringLen, 0) != (int) echoStringLen){
        	printf("Send failed! Different number of bytes then expected \n");
        	return;
      	}
	memset(echoBuffer, 0 , RCVBUFSIZE);
	while(strstr(echoBuffer,"//") == NULL) //Go until terminator.
	{
		if((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE, 0)) < 0){
		    printf("Connection closed early or recv() failure!");
		    return;
		  }
		if (strstr(echoBuffer, "//") == NULL){
			printf("%s",echoBuffer);
		}
		fflush(stdout);
	}
	printf("\n");
	memset(echoBuffer,0,RCVBUFSIZE); //reset in case.
	      	
	}
	
	else if(strcmp(echoString, "mkdir") == 0){
	  printf("What directory?: ");
	  scanf("%s", echoString);
	  echoStringLen = strlen(echoString);
   	  if(send(sock, echoString, echoStringLen, 0) != (int)echoStringLen){
	    printf("Send()  failed");
	    return;
	  }
	}
      
      else{
        /*Echo server*/
        totalBytesRcvd = 0;
	printf("Recieved: ");
	while (totalBytesRcvd < (int)echoStringLen){
	  if((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE, 0)) < 0){
            printf("Connection closed early or recv() failure!");
	    return;
	  }
	
        totalBytesRcvd += bytesRcvd;
	echoBuffer[bytesRcvd] = '\0';
	printf("%s", echoBuffer);
	}
		
	printf("\n");
	fflush(stdout);
      }
      
      printf("Command: ");
      scanf("%s",echoString);
      
    }

    //Exit client
    close(sock);
}


void SocketClient::ListDirectory(int sock)
{
    long Input = 0;
    char Directory[32];
    long totalBytesRcvd = 0;
    long bytesRcvd = 0;
	
    
      if((bytesRcvd = recv(sock, &Input, RCVBUFSIZE, 0)) < 0){
        printf("Connection closed early or recv() failure!");
     	return;
      }

      totalBytesRcvd += bytesRcvd;
      printf("File Size: %ld\n",Input);
      fflush(stdout);

      //Send that we got the length of file.
      if(send(sock, "ok", RCVBUFSIZE, 0) != RCVBUFSIZE){ 
        printf("Send failed! Different number of bytes then expected \n");
        return;
      }

      printf("Sent ok \n");
      fflush(stdout);
      totalBytesRcvd = 0;
      bytesRcvd = 0;
      while (totalBytesRcvd < Input){ //Get a long
	memset(Directory, 0, 32);        
	if((bytesRcvd = recv(sock, Directory, RCVBUFSIZE, 0)) < 0){
	  printf("Connection closed early can not get Directory!");
     	  return;
    	}
	if (((bytesRcvd + totalBytesRcvd) > Input) && (Input % 32 != 0)){	
		//Directory[Input - totalBytesRcvd] = '\0';
		for (int i = (Input % 32)-1; i < 32; i++){ //Trim packet. read gives an extra /n every time. If it's a 32 bit modulus, it will not be read by the recv anyways.
			Directory[i] = '\0';
		}
		
	}
     	totalBytesRcvd += bytesRcvd;
	//printf("| %ld |", bytesRcvd);
	
	printf("%s", Directory);
	fflush(stdout);
	

    }

    printf ("\n");
    printf ("%ld", totalBytesRcvd);	
    fflush(stdout);
}

int SocketClient::recvFile(long size, string savename,int port)
{
   ofstream outFile(savename.c_str(), ios::out | ios::binary | ios::app);
  
   
   char buffer[RCVBUFSIZE];
 
   long totalBytesRcvd = 0;
   long bytesRcvd = 0;

      printf("File Name: %s\n", savename.c_str());
      printf("File Size: %ld\n",size);
      fflush(stdout);

      //Send that we got the length of file.
      if(send(port, savename.c_str(), RCVBUFSIZE, 0) != RCVBUFSIZE){ 
        printf("Send failed! Different number of bytes then expected \n");
        return -1;
      }
   	
   while(totalBytesRcvd < size){
	memset(buffer,0, 32); //memset set to 0
	
	   
     if((bytesRcvd = recv(port, buffer, RCVBUFSIZE, 0)) < 0){
		      	
		printf("|%ld|", bytesRcvd);
		printf("RECIEVE ERROR");
		fflush(stdout);
		return -1; //Error.
      }
	   
      if (((bytesRcvd + totalBytesRcvd) > size) && (size % 32 != 0)){
         
        int i = (size % 32)-1; //Trim packet. read gives an extra /n every time. If it's a 32 bit modulus, it will be read by the recv anyways.
	outFile.write(buffer, i);		
	printf("%s|%ld|",buffer,totalBytesRcvd);
	totalBytesRcvd += bytesRcvd;    
	   }
      else{
	totalBytesRcvd += bytesRcvd; 
	outFile.write(buffer, bytesRcvd); // write data on to output file
        printf("%s|%ld|",buffer,totalBytesRcvd);
	}
   }
   printf("Transferred file to : %s\n", getcwd(buffer, RCVBUFSIZE));
   outFile.close();
   return 0;
}
