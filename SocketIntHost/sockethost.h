#ifndef SOCKETHOST_H
#define SOCKETHOST_H
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string>

using namespace std;

class SocketHost
{
public:
    SocketHost(int); //constructor, take ui and use
    void listDirectories(const char*);
    void changeDirectory(const char*, int);
    void makeDirectory(const char*, int);
    void HandleTCPClient(int);
    ~SocketHost();

protected:
    pid_t pid; //Process id.
    void Read(string, int);

};

#endif // SOCKETHOST_H
