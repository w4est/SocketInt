#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H
#include <string>

using namespace std;

class SocketClient
{
public:
    SocketClient(int,string);
private:
    void ListDirectory(int);
    int recvFile(long , string,int);
    void Read(string, int);
    long GetFileSize(string, int);
};

#endif // SOCKETCLIENT_H
