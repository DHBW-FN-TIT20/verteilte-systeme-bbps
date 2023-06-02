#include "server.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <vector>
using namespace std;

void handleClient(int clientSocket)
{
  char buffer[1024];
  int bytesRead;

  // Read data from the client
  while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
  {
    // Process the received data
    std::string receivedData(buffer, bytesRead);
    std::cout << "Received from client: " << receivedData << std::endl;
    // Prepare a response
    std::string response = "Hello client! I received your message.";

    // Send the response back to the client
    if (send(clientSocket, response.c_str(), response.length(), 0) < 0)
    {
      std::cerr << "Error sending response to client" << std::endl;
      break;
    }
  }

  if (bytesRead == 0)
  {
    // Client disconnected
    std::cout << "Client disconnected" << std::endl;
  }
  else if (bytesRead < 0)
  {
    // Error occurred during receiving
    std::cerr << "Error receiving data from client" << std::endl;
  }

  close(clientSocket);
}

Server::Server(int port, int topicTimeout)
{
  cout << "Starting server on port " << port << " with topic timeout of " << topicTimeout << " seconds." << endl;
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0)
  {
    std::cerr << "Error creating socket" << std::endl;
    return;
  }

  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
  {
    std::cerr << "Error binding socket" << std::endl;
    return;
  }

  int backlog = 5; // Maximum number of pending connections
  if (listen(serverSocket, backlog) < 0)
  {
    std::cerr << "Error listening for connections" << std::endl;
    return;
  }

  std::vector<std::thread> clientThreads;

  while (true)
  {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (clientSocket < 0)
    {
      std::cerr << "Error accepting connection" << std::endl;
      continue;
    }

    // Create a new thread to handle the client connection
    // std::thread threadObj(handleClient, clientSocket);
    // clientThreads.push_back(std::move(threadObj));
    handleClient(clientSocket);
  }

  // Join all the client threads
  // for (auto &threadObj : clientThreads)
  // {
  //   threadObj.join();
  // }

  close(serverSocket);
}

Server::~Server()
{
  cout << "Stopping server" << endl;
}