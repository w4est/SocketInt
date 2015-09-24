#include "sockethost.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#include <dirent.h>

using namespace std;

SocketHost::SocketHost(MainWindow* Window)
{
    void HandleTCPClient(int clntSocket); //TODO MOVE THIS INTO A CLASS


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
    }

    echoServPort = atoi(port.c_str()); //Convert string to int

    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {

         Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() + "Socket failed to create, Exiting.\n");
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
    }

    Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() +"Socket Bound.\n");

    //Mark the socket so it will listen
    if (listen(servSock, 5 /*Maximum*/) < 0)
    {
        //TODO DIE WITH ERROR LISTEN FAILED
        Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() +"Listening Error. Exiting.\n");
    }

    Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() +"Listening.\n");

    while (0){ //Run Forever!

        //Set the size of the in-out parameter
        clntLen = sizeof(echoClntAddr);

        //wait for a client to connect
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) <0 )
        {
            //DIE WITH ERROR Accept failed
            printf("Die failed");
        }

        //clntSock is connected to a client!
        Window->ui->ConsoleText->setText(Window->ui->ConsoleText->text() + "Handling client" + inet_ntoa(echoClntAddr.sin_addr));

        //HandleTCPClient(clntSock); //Make this function!
        close(servSock); //Need to move to another file to make this work!
    }

    //If it reaches here. we are doomed.




}

//create a file dirList.txt for the server to read from and send line by line
void SocketHost::listDirectories(){ //called when the server receives an ls command
    const char* path = ".";

    FILE* dirList = fopen("dirList.txt", "w");
    DIR* d = opendir(path);
    if (d == NULL) return;

    for(struct dirent *de = NULL; (de = readdir(d)) != NULL; ){
      fputs(de->d_name, dirList);
      fputs("\n", dirList);
    }
    closedir(d);
    fclose(dirList);
}
