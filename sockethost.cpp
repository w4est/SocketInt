#include "sockethost.h"
#include "ui_mainwindow.h"
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

SocketHost::SocketHost(MainWindow* Window)
{



    int servSock; /* server socket */
    int clntSock; /* client socket */
    string port; //Create a string to hold the port number
    port = (Window->ui->HostPort->toPlainText()).toStdString(); //Convert from the text field

    struct sockaddr_in echoServAddr; /*server address struct*/
    struct sockaddr_in echoClntAddr; /*Destination (Client) address struct*/
    unsigned short echoServPort; //server port
    unsigned int clntLen; // Length of Client address data structure

    Window->ui->ConsoleText->setText("The host button is doing something.\n");

    if (atoi (port.c_str()) <= 0){ //Make sure that the port number is usable (>-1)
        //This isn't going to work.
        printf("Error, invalid port number");
        return;
    }

    echoServPort = atoi(port.c_str()); //Convert string to int

    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {

         Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() + "Socket failed to create, Exiting.\n");
         return;
    }

    Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() + "Socket Created at " + port.c_str() + ".\n");

    //Construct Local Address structture
    memset(&echoServAddr,0,sizeof(echoServAddr)); //Zero out the structure
    echoServAddr.sin_family = AF_INET; // Internet address family
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); //Any Incoming interface
    echoServAddr.sin_port = htons(echoServPort); // Local Port

    //Bind to local Address
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
    {
        Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() +"Bind Error, Exiting. \n");
        return;
    }

    Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() +"Socket Bound.\n");

    //Mark the socket so it will listen
    if (listen(servSock, 5 /*Maximum*/) < 0)
    {

        Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() +"Listening Error. Exiting.\n");
        close(servSock); //close binded socket
        return;
    }

    Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() +"Listening.\n");
    fflush(stdout);
    pid = fork();

    if (pid == 0)
    {
        printf("child created\n");
        fflush(stdout);
        while (1){ //Run Forever!
            Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() +"Still Listening. \n");

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
            Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() + "Handling client" + inet_ntoa(echoClntAddr.sin_addr));

            HandleTCPClient(clntSock); //Deal with client.
            close(servSock);
            exit(0);

        }
    }
    else{
      //DO NOTHING
        printf("I am the parent, and I am done creating a child \n");
        fflush(stdout);
        close(servSock);
    }


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

void SocketHost::changeDirectory(const char *currDir){
    chdir(currDir); //changes the directory FOR THE CURRENT WORKING PROCESS
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

    if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        printf("Recv failed");
    while (recvMsgSize > 0){
        if(send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
            printf("Request failed");
        if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
            printf("recv() failed");
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
