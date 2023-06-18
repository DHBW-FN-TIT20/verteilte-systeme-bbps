#include <spdlog/spdlog.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "clientconnection.h"
#include "../shared/message.h"

using namespace std;

ClientConnection::ClientConnection(string address, int port)
{
    spdlog::debug("Creating client connection on port {} with server address {}.", port, address);
    this->address = address;
    this->port = port;
}

ClientConnection::~ClientConnection()
{
    spdlog::debug("Destroying client connection {}:{}", this->address, this->port);
}

void ClientConnection::SendMessage(string topicName, string message, time_t timestamp)
{
    // Send message to client
    // Connect to the client which has a socket listening on the port

    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0)
    {
        spdlog::error("Error creating socket");
        return;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->port);
    serverAddress.sin_addr.s_addr = inet_addr(this->address.c_str());


    // Create the response
    Message messageObj = Message(topicName, timestamp, message);
    string serializedMessage = messageObj.serialize();


    // Send the response
    sendto(clientSocket, (const char *)serializedMessage.c_str(), serializedMessage.length(), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    return;
}

string ClientConnection::getAddress()
{
    return this->address;
}

int ClientConnection::getPort()
{
    return this->port;
}
