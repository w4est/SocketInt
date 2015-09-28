#include "socketclient.h"
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr.in and inet.addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close */
#include <sys/types.h>
#include <signal.h>
#include <clientform.h>
#include <QApplication>

#define RCVBUFSIZE 32 /*Size ofreceive buff */




SocketClient::SocketClient(int Port)
{
    pid_t pid; //Variable for new process
    int sock;
    struct sockaddr_in echoServAddr;
    unsigned short echoServPort = Port; //Make sure we are using the server port here!
    char *servIP;
    char *echoString;
    char echoBuffer[RCVBUFSIZE];
    unsigned int echoStringLen;
    int bytesRcvd, totalBytesRcvd;

    servIP = "127.0.0.1"; //TODO make these come from form
    echoString = "Hello";
    //Create a new process so we don't process block, buttons only make threads

    //pid = fork();

    //if (pid == 0)
      //  {
        //ClientForm form; //Create socket form
        //form.show(); //Doesn't work?

        if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
           {
            printf("Socket failed to create! \n");
            return;

        }
        memset(&echoServAddr, 0, sizeof(echoServAddr));
        echoServAddr.sin_family = AF_INET;
        echoServAddr.sin_addr.s_addr = inet_addr(servIP);
        echoServAddr.sin_port = htons(echoServPort + 1);




        if(connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
            {

            printf("connect failed! \n");
            fflush(stdout);
            return;

        }

        echoStringLen = strlen(echoString);

        if(send(sock, echoString, echoStringLen, 0) != echoStringLen)
            {
            printf("Send failed! Different number of bytes then expected \n");
            return;
        }

        totalBytesRcvd = 0;
        printf("Recieved: ");
        while (totalBytesRcvd < echoStringLen){
           if((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <=0)
             {
             printf("Connection closed early or recv() failure!");
             return;
            }
             totalBytesRcvd += bytesRcvd;
             echoBuffer[bytesRcvd] = '\0';
             printf(echoBuffer);
        }

        printf("\n");
        fflush(stdout);
        close(sock);
        //kill(pid, SIGTERM);
        //exit(0);


    //}
    //else if (pid == -1)
    //{
       // printf("Process fork error!");
   // }
    //else{
        //printf("Keeping the form running");
   // }



}

