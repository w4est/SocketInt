#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

using namespace std;

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
