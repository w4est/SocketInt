#include "sockethost.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    int port;
    cout << "What port would you like to use?" << endl;
    cin >> port;
    SocketHost Host(port);
}
