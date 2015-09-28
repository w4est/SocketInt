#ifndef SOCKETHOST_H
#define SOCKETHOST_H
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <dirent.h>

class SocketHost
{
public:
    SocketHost(MainWindow*); //constructor, take ui and use
    void listDirectories(const char*);
    void changeDirectory(const char*);
    void makeDirectory(const char*);
    void HandleTCPClient(int clntSocket);
    ~SocketHost();

protected:
    pid_t pid; //Process id.


};

#endif // SOCKETHOST_H
