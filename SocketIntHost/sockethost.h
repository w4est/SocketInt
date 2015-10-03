#ifndef SOCKETHOST_H
#define SOCKETHOST_H
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>

class SocketHost
{
public:
    SocketHost(int); //constructor, take ui and use
    void listDirectories(const char*);
    char* changeDirectory(const char*, const char*);
    void makeDirectory(const char*);
    void HandleTCPClient(int);
    ~SocketHost();

protected:
    pid_t pid; //Process id.
    void Read(FILE*, int);

};

#endif // SOCKETHOST_H
