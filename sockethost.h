#ifndef SOCKETHOST_H
#define SOCKETHOST_H
#include "mainwindow.h"
#include "ui_mainwindow.h"


class SocketHost
{
public:
    SocketHost(MainWindow*); //constructor, take ui and use
    void listDirectories();
};

#endif // SOCKETHOST_H
