#include "sockethost.h"
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fstream>
#include <limits.h>

#define RCVBUFSIZE 32

using namespace std;

SocketHost::SocketHost(int port)
{




    int servSock; /* server socket */
    int clntSock; /* client socket */

    struct sockaddr_in echoServAddr; /*server address struct*/
    struct sockaddr_in echoClntAddr; /*Destination (Client) address struct*/
    unsigned short echoServPort; //server port
    unsigned int clntLen; // Length of Client address data structure


    if (port <= 0){ //Make sure that the port number is usable (>-1)
        //This isn't going to work.
        printf("Error, invalid port number");
        return;
    }

    echoServPort = port; //ASsign port

    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {

         cout << "Socket failed to create, Exiting." << endl;
         return;
    }

    cout << "Socket Created at " << port << endl;

    //Construct Local Address structture
    memset(&echoServAddr,0,sizeof(echoServAddr)); //Zero out the structure
    echoServAddr.sin_family = AF_INET; // Internet address family
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); //Any Incoming interface
    echoServAddr.sin_port = htons(echoServPort); // Local Port

    //Bind to local Address
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
    {
        cout << "Bind Error, Exiting." << endl;
        return;
    }

   cout << "Socket Bound." << endl;

    //Mark the socket so it will listen
    if (listen(servSock, 10 /*Maximum*/) < 0)
    {

        cout << "Listening Error. Exiting." << endl;
        close(servSock); //close binded socket
        return;
    }

    cout << "Listening." << endl;
    fflush(stdout);
    while (1){ //Run Forever!
            cout << "Still Listening." << endl;

            //Set the size of the in-out parameter
            clntLen = sizeof(echoClntAddr);

            //wait for a client to connect
            if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) <0 )
            {
                //Accept failed
                printf("Accept failed");
                fflush(stdout);
                close(servSock);
                exit(0);
            }

            //clntSock is connected to a client!
            cout << "Handling client" << inet_ntoa(echoClntAddr.sin_addr) << endl;

            HandleTCPClient(clntSock); //Deal with client.
	}
      	close(servSock);
	exit(0);
}

//create a file dirList.txt for the server to read from and send line by line
void SocketHost::listDirectories(const char* currDir){ //called when the server receives an ls command
    FILE* dirList = fopen("dirList.txt", "w+"); //overwrite the file
    DIR* d = opendir(currDir);
    if (d == NULL) return;

    for(struct dirent *de = NULL; (de = readdir(d)) != NULL; ){
      fputs(de->d_name, dirList);
      fputs("\n", dirList);
    }
    closedir(d);
    fclose(dirList);
}

void SocketHost::changeDirectory(const char *newDir, int sock){
    char buffer[PATH_MAX+1];   
    char packet[RCVBUFSIZE];

    int count = 0;

    chdir(newDir); //changes the directory FOR THE CURRENT WORKING PROCESS
    
    if(getcwd(buffer, sizeof(buffer)) != NULL){
	
	while((count*32) < (int)strlen(buffer)) //For every char
  	{
        memset(packet,0, RCVBUFSIZE); //Set packet to zero
	if ((strlen(buffer) - (count*32)) >= 32) //Packetize the buffer, and send it.
	{
		for (int i = 0; i < 32; i++)
		{
			packet[i] = buffer[(count*32) + i];
				
		}
		if(send(sock, packet, RCVBUFSIZE, 0) < 0){
		   printf("Send() falied");
		   return;
		}
		printf("%s",packet);
	}
	else{
		int x = strlen(buffer) - (count*32);
		for (int i = 0; i < x; i++)
		{
			packet[i] = buffer[(count*32) + i];
				
		}
		if(send(sock, packet, RCVBUFSIZE, 0) < 0){
		   printf("Send() failed");
		   return;
		}
		
		
		}
	count++;
		}
	
     
   	 }
	
	if(send(sock, "//", RCVBUFSIZE, 0) < 0){ //Send Acknowledgement, the client needs
	   printf("send failed()");
	}
}

void SocketHost::makeDirectory(const char *currDir, int sock){
    struct stat st = {0};
    char buffer[PATH_MAX+1];

    if(stat(currDir, &st) == -1){
      mkdir(currDir, 0700);
     printf("made directory: %s\n", currDir);
    }

    if(send(sock, buffer, sizeof(buffer), 0) < 0)
      printf("send() failed");
    
}

void SocketHost::HandleTCPClient(int clntSocket){
    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize = 1;
    char cwd[1024];
    long fileSize;
    

    
    while (recvMsgSize > 0){   //Loop. until told to die. (TODO) MAKE DIE command
        

        if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
            printf("recv failed");
        echoBuffer[recvMsgSize] = '\0';
	printf("Received: %s\n" , echoBuffer);

	//handle mkdir
	if(strcmp(echoBuffer, "mkdir") == 0){
	    printf("echoBuffer: %s\n", echoBuffer);
	    if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
	      printf("recv() failed");
            printf("Made to: %s\n", echoBuffer);
            makeDirectory(echoBuffer, clntSocket);
	}
		
	//handle cd
	if(strcmp(echoBuffer, "cd") == 0){
	  printf("You have chosen to change the directory\n");
        
	  if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)//get second word
            printf("recv() failed");
          printf("Changed to: %s\n", echoBuffer);
          changeDirectory(echoBuffer, clntSocket);
	  
	}

	//handle ls
	else if(strcmp(echoBuffer, "ls") == 0){
	    fileSize= 0;
	    listDirectories(getcwd(cwd, sizeof(cwd)));
	    FILE* dirList = fopen("dirList.txt", "r");
	    fseek(dirList, 0L, SEEK_END);
            fileSize = ftell(dirList);
	    if(send(clntSocket, &fileSize, RCVBUFSIZE, 0) != RCVBUFSIZE){
                printf("Send() failed");
            }
	    printf("File Size: %ld\n: ", fileSize);
            if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0 )
	        printf("recv() failed");
            echoBuffer[recvMsgSize] = '\0';
            
            if(strcmp(echoBuffer, "ok") == 0)
                printf("Received the ok\n");
            fflush(stdout);
            fseek(dirList, 0, SEEK_SET);
	    memset(echoBuffer,0,sizeof(echoBuffer));
	   fclose(dirList);

	   Read("dirList.txt", clntSocket); //Read and send!

	  
	}

	//Handle file transfer
	else if(strcmp(echoBuffer, "get") == 0){
 		  fileSize = 0;

		  printf("File Requested\n");
		  //get file name
		  memset(echoBuffer, 0, RCVBUFSIZE);
		  if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE-1, 0)) < 0)
		   { 
			printf("recv() failed");
			return;
		  }
		//echoBuffer[recvMsgSize] = '\0';
		printf("Requested: %s\n", echoBuffer);
		  
		fileSize = GetFileSize(echoBuffer, clntSocket);
		
		
		if ( fileSize > 0) //If file exists
		{
			if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
	        		printf("recv() failed");
            		//echoBuffer[recvMsgSize] = '\0';
            		printf("%s\n",echoBuffer);
                	printf("Starting Transfer\n");
            		fflush(stdout);
			Read(echoBuffer,clntSocket); //Send file.
		}
		 else
		{
			printf("Client requested file that does not exist!\n");
			if(send(clntSocket, &fileSize, sizeof(long), 0) != sizeof(long)){
	  		 	printf("Send() Directory failed");
				return;
			}
		}  
		


	}

	//Handle file being uploaded
	else if(strcmp(echoBuffer, "send") == 0){
 		  fileSize = 0;

		  printf("File Requested\n");
		  //get file name
		  memset(echoBuffer, 0, RCVBUFSIZE);
		  if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE-1, 0)) < 0)
		   { 
			printf("recv() failed");
			return;
		  }
		echoBuffer[recvMsgSize] = '\0';
		printf("Requested to store: %s\n", echoBuffer);
		  
		if((recvMsgSize = recv(clntSocket, &fileSize, RCVBUFSIZE, 0)) < 0)
	        		printf("recv() failed");
		
		
		if ( fileSize > 0) //If file exists
		{
			
                	printf("Being Sent file\n");
			recvFile(fileSize,echoBuffer,clntSocket); //Recieve this file.
		}
		 else
		{
			printf("Null file received that does not exist!\n"); //If it cannot be sent, don't wait for it.
			
		}  
		


	}

	//Parrot back any unknown commands.        
	else{
	if(send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
            {printf("Request failed");
		return;
	    }
	}
    }
   close(clntSocket);
}

void SocketHost::Read(string file, int sock){ //Read file in 32 bytes packets and send.

  int READ_SIZE = 32;

  ifstream input(file.c_str(), ios::in | ios::binary);
  
  if(input.fail() == true){
	printf("Cannot open file to read!");
	fflush(stdout);
	return;
	}

  // get the file length
  input.seekg(0, input.end);
  long length = input.tellg();
  input.seekg(0, input.beg); 

  

  char*  buffer = new char[32];
  cout << "File Length: " << length << endl;
  cout << "Reading 32-bytes at a time...\n";
  
   //Alternative method. unneeded.
  char*  filedata = new char[length];
  input.read(filedata, length); //Get file into a char array
  input.close();
  long count = 0;

  while((count*32) < length) //For every char
  {
        memset(buffer,0, READ_SIZE); //Set packet to zero
	if ((length - (count*32)) >= 32)
	{
		for (int i = 0; i < 32; i++)
		{
			buffer[i] = filedata[(count*32) + i];
				
		}
		if(send(sock, buffer, RCVBUFSIZE, 0) < 0){
		   printf("Send() Directory falied");
		   return;
		}
		count++;
		//printf("%s |%ld|",buffer,count*32);
	}
	else{
		int x = length - (count*32);
		for (int i = 0; i < x; i++)
		{
			buffer[i] = filedata[(count*32) + i];
				
		}
		if(send(sock, buffer, RCVBUFSIZE, 0) < 0){
		   printf("Send() Directory failed");
		   return;
		}
		
		count++;
		//printf("%s |%ld|",buffer,count*32);
	}

	

  }

  


}
//Gets Filesize and sends to client
long SocketHost::GetFileSize(string filename, int port){ 


  //Create ifstream, and open file.
  ifstream input(filename.c_str(), ios::in | ios::binary);
  
  if (input.fail() == true) //If opening file had error
  {
	printf("File cannot be opened");
	fflush(stdout);
	return 0; //Return nothing.
  }
  // get the file length
  input.seekg(0, input.end);
  long length = input.tellg();
  input.seekg(0,input.beg);
  input.close(); //Close file

  

	//Send to client
	if(send(port, &length, sizeof(long), 0) != sizeof(long)){
	   printf("Send() Directory failed");
	   return 0;
	}
  return length; //Return file length.
}


int SocketHost::recvFile(long size, string savename,int port)
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
		printf("RECIEVE ERROR, Cannot get file!");
		fflush(stdout);
		return -1; //Error.
      }
	   
      if (((bytesRcvd + totalBytesRcvd) > size) && (size % 32 != 0)){
         
        int i = (size % 32)-1; //Trim packet. read gives an extra /n every time. If it's a 32 bit modulus, it will be read by the recv anyways.
	outFile.write(buffer, i);		
	//printf("%s|%ld|",buffer,totalBytesRcvd);
	totalBytesRcvd += bytesRcvd;    
	   }
      else{
	totalBytesRcvd += bytesRcvd; 
	outFile.write(buffer, bytesRcvd); // write data on to output file
        //printf("%s|%ld|",buffer,totalBytesRcvd);
	}
   }
   printf("Transferred file to host working directory");
   outFile.close();
   return 0;
}
