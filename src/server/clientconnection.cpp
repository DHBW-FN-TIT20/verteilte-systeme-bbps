#include "clientconnection.h"
#include <iostream>
using namespace std;

ClientConnection::ClientConnection(string address, int port){
  cout << "Creating client connection on port " << port << " with server address " << address << endl;
}

ClientConnection::~ClientConnection(){
  cout << "Destroying client connection" << endl;
}