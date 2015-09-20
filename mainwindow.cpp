#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

using namespace std;

#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr.in and inet.addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close */
#include <sys/types.h>

#define RCVBUFSIZE 32 /*Size ofreceive buff */

void DieWithError(char* errorMessage);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ClientButton_clicked()
{
//    int sock;
//    struct sockaddr_in echoServAddr;
//    unsigned short echoServPort;
//    char *servIP;
//    char *echoString;
//    char echoBuffer[RCVBUFSIZE];
//    unsigned int echoStringLen;
//    int bytesRcvd, totalBytesRcvd;

//    servIP = "134.117.249.83";
//    echoString = "Hello";

//    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
//        DieWithError("socket() failed");

//    memset(&echoServAddr, 0, sizeof(echoServAddr));
//    echoServAddr.sin_family = AF_INET;
//    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
//    echoServAddr.sin_port = htons(echoServPort);

//    if(connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
//        DieWithError("connect() failed");

//    echoStringLen = strlen(echoString);

//    if(send(sock, echoString, echoStringLen, 0) != echoStringLen)
//        DieWithError("send() sent a different number of bytes than expected");

//    totalBytesRcvd = 0;
//    printf("Recieved: ");
//    while (totalBytesRcvd < echoStringLen){
//        if((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <=0)
//            DieWithError("recv() failed or connection closed prematurely");
//        totalBytesRcvd += bytesRcvd;
//        echoBuffer[bytesRcvd] = '\0';
//        printf(echoBuffer);
//    }

//    printf("\n");
//    close(sock);
//    exit(0);
}

void MainWindow::on_HostButton_clicked()
{
    void HandleTCPClient(int clntSocket); //TODO MOVE THIS INTO A CLASS


    int servSock; /* server socket */
    int clntSock; /* client socket */
    char* port = "3000";

    struct sockaddr_in echoServAddr; /*server address struct*/
    struct sockaddr_in echoClntAddr; /*Destination (Client) address struct*/
    unsigned short echoServPort; //server port
    unsigned int clntLen; // Length of Client address data structure

    ui->ConsoleText->setText("The host button is doing something.");

    if ((int) port < 0){
        //Shit.
    }

    echoServPort = atoi(port); //Take First Arg TODO: take from text box

    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {

         ui->ConsoleText->setText(ui->ConsoleText->text() + "Socket failed to create, Exiting.\n");
    }

    ui->ConsoleText->setText(ui->ConsoleText->text() + "Socket Created.\n");

    //Construct Local Address structture
    memset(&echoServAddr,0,sizeof(echoServAddr)); //Zero out the structure
    echoServAddr.sin_family = AF_INET; // Internet address family
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); //Any Incoming interface
    echoServAddr.sin_port = htons(echoServPort); // Local Port

    //Bind to local Address
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
    {
        ui->ConsoleText->setText(ui->ConsoleText->text() +"Bind Error, Exiting. \n");
    }

    ui->ConsoleText->setText(ui->ConsoleText->text() +"Socket Bound.\n");

    //Mark the socket so it will listen
    if (listen(servSock, 5 /*Maximum*/) < 0)
    {
        //TODO DIE WITH ERROR LISTEN FAILED
        ui->ConsoleText->setText(ui->ConsoleText->text() +"Listening Error. Exiting.\n");
    }

    ui->ConsoleText->setText(ui->ConsoleText->text() +"Listening.\n");

    while (0){ //Run Forever!

        //Set the size of the in-out parameter
        clntLen = sizeof(echoClntAddr);

        //wait for a client to connect
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) <0 )
        {
            //DIE WITH ERROR Accept failed

        }

        //clntSock is connected to a client!
        cout << "Handling client" << inet_ntoa(echoClntAddr.sin_addr);

        HandleTCPClient(clntSock);

    }

    //If it reaches here. we are doomed.



}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}
