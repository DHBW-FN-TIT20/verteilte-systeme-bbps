#include "server.h"
#include <iostream>
using namespace std;

Server::Server(int port, int topicTimeout){
  cout << "Starting server on port " << port << " with topic timeout of " << topicTimeout << " seconds." << endl;
}

Server::~Server(){
  cout << "Stopping server" << endl;
}