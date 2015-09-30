#include "socketclient.h"
#include <iostream>
#include <string.h>

using namespace std;

int main(int argc, char *argv[])
{
    string ip;
    cout << "What ip address would you like to connect to?" << endl;
    getline(cin,ip); //Get string
    int port;
    cout << "What port is the server using?" << endl;
    cin >> port;
    SocketClient client(port, ip);
}
