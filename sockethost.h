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
};

#endif // SOCKETHOST_H
