#include "sockethost.h"
#include <signal.h>
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

char* SocketHost::changeDirectory(const char *currDir, const char *subDir){
    char* newDir ="";
    strcpy(newDir, currDir);
    strcat(newDir, subDir);
    chdir(currDir); //changes the directory FOR THE CURRENT WORKING PROCESS
    return newDir;
}

void SocketHost::makeDirectory(const char *currDir){
    struct stat st = {0};
    if(stat(currDir, &st) ==-1){
        mkdir(currDir, 0700);
    }
}

void SocketHost::HandleTCPClient(int clntSocket){
    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize;
    char cwd[1024];
    long int fileSize;

    if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        printf("Recv failed");
    while (recvMsgSize > 0){
        if(send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
            printf("Request failed");
        if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
            printf("recv() failed");
	echoBuffer[recvMsgSize] = '\0';
	printf("Received: %s\n" , echoBuffer);
	if(strcmp(echoBuffer, "ls") == 0){
	    listDirectories(getcwd(cwd, sizeof(cwd)));
	    FILE* dirList = fopen("dirList.txt", "r");
	    fseek(dirList, 0L, SEEK_END);
            fileSize = ftell(dirList);
	    if(send(clntSocket, &fileSize, RCVBUFSIZE, 0) < 0){
                printf("Send() failed");
            }
	    printf("File Size: %ld\n: ", fileSize);
            if((recvMsgSize = recv(clntSocket, echoBuffer,RCVBUFSIZE, 0)) < 0 )
	        printf("recv() failed");
            if(strcmp(echoBuffer, "ok"))
                printf("Received the ok");
	    while(fgets(echoBuffer, RCVBUFSIZE, dirList)){
	      if(send(clntSocket, echoBuffer, RCVBUFSIZE, 0) < 0)
	        printf("Send() falied");
            }
	  fclose(dirList);
	}
    }
   close(clntSocket);
}

SocketHost::~SocketHost(){
//    if (pid > 0){ // If parent, clean up the child process.
//      kill(pid,SIGKILL);
//      printf("Killing child");
//      fflush(stdout);
//    }

}
