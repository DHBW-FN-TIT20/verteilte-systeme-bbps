#include "clientconnection.h"
#include "../shared/response.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

ClientConnection::ClientConnection(string address, int port)
{
    cout << "Creating client connection on port " << port << " with server address " << address << endl;
}

ClientConnection::~ClientConnection()
{
    cout << "Destroying client connection" << endl;
}

void ClientConnection::SendMessage(string topicName, string message, time_t timestamp)
{
    // Send message to client
    // Connect to the client which has a socket listening on the port

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        cerr << "Error creating socket" << endl;
        return;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->port);
    serverAddress.sin_addr.s_addr = inet_addr(this->address.c_str());

    int connectStatus = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectStatus < 0)
    {
        cerr << "Error connecting to the socket" << endl;
        close(clientSocket);
        return;
    }

    string serializedResonse;
    // Create the response
    Response response = Response(CommandIdentifiers::publish);
    if(response.setResponseArgument(CommunicationParameters::message, message) &&
        response.setResponseArgument(CommunicationParameters::topicName, topicName) &&
        response.setResponseArgument(CommunicationParameters::messageTimestamp, to_string(timestamp)))
    {
        // Serialize the response
        serializedResonse = response.serialize();
    }
    else
    {
        // Error creating the response
        cerr << "Error creating the response" << endl;
        close(clientSocket);
        return;
    }
    
    // Send the response
    int sendStatus = send(clientSocket, serializedResonse.c_str(), serializedResonse.length(), 0);
    if (sendStatus < 0)
    {
        cerr << "Error sending data" << endl;
        close(clientSocket);
    }

    // Close the socket
    close(clientSocket);

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
