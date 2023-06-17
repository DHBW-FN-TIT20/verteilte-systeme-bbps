#include "server.h"
#include "../shared/command.h"
#include "../shared/statuscode.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <algorithm>

using namespace std;

void Server::handleApproachingClient(int clientSocket, struct sockaddr_in *clientAddress)
{
    char buffer[1024];

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0)
    {
        cerr << "Error receiving data from client" << endl;
        close(clientSocket);
        return;
    }
    // Process the received data
    // TODO remove for Production
    string receivedData(buffer, bytesRead);
    cout << "Received from client: " << receivedData << std::endl;

    // deserialize the received data
    // try catch to catch the exception if the string is not a valid json
    Command command;
    try
    {
        command = Command::deserialize(receivedData);
    }
    catch (const exception &e)
    {
        cerr << e.what() << endl;
        // TODO Return internal parameter error
    }

    // get Ip Address of the client
    string clientIpAddress = inet_ntoa(clientAddress->sin_addr);

    // TODO remove for Production
    cout << "Client IP Address: " << clientIpAddress << endl;

    Response response;

    switch (command.commandIdentifier)
    {
    case CommandIdentifiers::subscribe:
        response = this->handleSubsscribeRequest(clientIpAddress, stoi(command.getCommandArgument(CommunicationParameters::clientPort)), command.getCommandArgument(CommunicationParameters::topicName));
        break;
    case CommandIdentifiers::unsubscribe:
        response = this->handleUnsubscribeRequest(clientIpAddress, stoi(command.getCommandArgument(CommunicationParameters::clientPort)), command.getCommandArgument(CommunicationParameters::topicName));
        break;
    case CommandIdentifiers::listTopics:
        response = this->handleListTopics();
        break;
    case CommandIdentifiers::getTopicStatus:
        response = this->handleGetTopicStatus(command.getCommandArgument(CommunicationParameters::topicName));
        break;
    case CommandIdentifiers::publish:
        response = this->handlePublishRequest(command.getCommandArgument(CommunicationParameters::topicName), command.getCommandArgument(CommunicationParameters::message));
    default:
        break;
    }

    // serialize the response
    string serializedResponse = response.serialize();

    // send the response to the client
    int bytesSent = send(clientSocket, serializedResponse.c_str(), serializedResponse.length(), 0);
    if (bytesSent != serializedResponse.length())
    {
        cerr << "Error sending data to client" << endl;
        close(clientSocket);
        return;
    }

    // close the connection
    close(clientSocket);
}

void Server::startServer(int port, int topicTimeout)
{
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

        // Create a new thread to handle the approaching client connection
        std::thread threadObj(&Server::handleApproachingClient, this, clientSocket, &clientAddress);
        clientThreads.push_back(std::move(threadObj));
    }

    // Code wont be executed
    // TODO Handle end of Server on ctrl + c

    // //Join all the client threads
    // for (auto &threadObj : clientThreads)
    // {
    //   threadObj.join();
    // }

    // close(serverSocket);
}

Server::Server(int port, int topicTimeout)
{
    cout << "Starting server on port " << port << " with topic timeout of " << topicTimeout << " seconds." << endl;

    // Start the Server in a while loop
    startServer(port, topicTimeout);
}

Server::~Server()
{
    cout << "Stopping server" << endl;
}

Response Server::handleSubsscribeRequest(string ipAddress, int port, string topicName)
{
    // Create Response object
    Response response = Response(CommandIdentifiers::subscribe);
    // find the topic with the given name
    Topic *topicPtr = nullptr;
    for (Topic *topic : this->topics)
    {
        if (topic->getTopicName() == topicName)
        {
            topicPtr = topic;
            break;
        }
    }

    // if the topic does not exist return invalid parameter
    if (topicPtr == nullptr)
    {
        response.setStatusCode(Statuscode::invalidParameter);
        return response;
    }

    // check if the ClientConnection already exists
    ClientConnection *clientConnectionPtr = nullptr;
    for (ClientConnection *clientConnection : this->clientConnections)
    {
        if (clientConnection->getAddress() == ipAddress && clientConnection->getPort() == port)
        {
            clientConnectionPtr = clientConnection;
            break;
        }
    }

    if (clientConnectionPtr != nullptr)
    {
        // Create the ClientConnection
        ClientConnection clientConnection(ipAddress, port);
        // Add to the ClientConnections
        this->clientConnections.push_back(&clientConnection);
        // Set the Pointer
        clientConnectionPtr = &clientConnection;
    }

    // Set response Arguments / Add the client to the topic
    // subscribe returns true if the client was added to the topic successfully and false if the client was already subscribed
    if (topicPtr->subscribe(clientConnectionPtr))
    {
        // Set status code to success
        response.setStatusCode(Statuscode::success);
    }
    else
    {
        // Set status code to failed
        response.setStatusCode(Statuscode::failed);
    }
    return response;
}

Response Server::handleUnsubscribeRequest(string ipAddress, int port, string topicName)
{
    // find the client with the given ip and port
    ClientConnection *clientConnectionPtr = nullptr;
    for (ClientConnection *clientConnection : this->clientConnections)
    {
        if (clientConnection->getAddress() == ipAddress && clientConnection->getPort() == port)
        {
            clientConnectionPtr = clientConnection;
            break;
        }
    }

    // get the topic with the given name
    Topic *topicPtr = nullptr;
    for (Topic *topic : this->topics)
    {
        if (topic->getTopicName() == topicName)
        {
            topicPtr = topic;
            break;
        }
    }
    // Create Response object
    Response response = Response(CommandIdentifiers::unsubscribe);

    // if the topic does not exist return invalid parameter
    if (topicPtr == nullptr)
    {
        response.setStatusCode(Statuscode::invalidParameter);
        return response;
    }
    
    if (topicPtr->unsubscribe(clientConnectionPtr))
    {
        // Set status code to success
        response.setStatusCode(Statuscode::success);
    }
    else
    {
        // Set status code to failed
        response.setStatusCode(Statuscode::failed);
        return response;
    }

    // check if the client is subscribed to any other topic
    bool isSubscribedToOtherTopic = false;
    for (Topic *topic : this->topics)
    {
        if (topic->hasClient(clientConnectionPtr->getAddress(), clientConnectionPtr->getPort()))
        {
            isSubscribedToOtherTopic = true;
            break;
        }
    }

    if (!isSubscribedToOtherTopic)
    {
        // if not, remove the client from the clientConnections
        // TODO Test it
        clientConnections.erase(std::remove(clientConnections.begin(), clientConnections.end(), clientConnectionPtr), clientConnections.end());
    }

    return response;
}

Response Server::handleListTopics()
{
    // Create Response object
    Response response = Response(CommandIdentifiers::listTopics);

    // Cast the Topics in vector of strings
    vector<string> topicNames;
    for (Topic *topic : this->topics)
    {
        topicNames.push_back(topic->getTopicName());
    }

    // Set response Arguments
    if (response.setResponseArgument(CommunicationParameters::topics, topicNames))
    {
        // Set status code to success
        response.setStatusCode(Statuscode::success);
    }
    else
    {
        // Set status code to internal server error
        response.setStatusCode(Statuscode::internalError);
    }
    return response;
}

Response Server::handleGetTopicStatus(string topicName)
{
    // Create Response object
    Response response = Response(CommandIdentifiers::getTopicStatus);

    // init pointer to topic
    Topic *topicPtr = nullptr;

    // Find the topic with the given name
    for (Topic *topic : this->topics)
    {
        if (topic->getTopicName() == topicName)
        {
            topicPtr = topic;
            break;
        }
    }

    // Check if the topic was found
    if (topicPtr == nullptr)
    {
        // Set status code to failed
        response.setStatusCode(Statuscode::failed);
        return response;
    }

    // get message timestamp and cast to int64_t
    int64_t messageTimestamp = topicPtr->getMessageTimestamp();

    // get list of Subscribers as vector of ClientConnections
    vector<ClientConnection *> connections = topicPtr->getClientConnections();

    // cast the IP Adresses in a vector of strings
    vector<string> subscribers;
    for (ClientConnection *connection : connections)
    {
        subscribers.push_back(connection->getAddress());
    }

    // Set response Arguments
    if (response.setResponseArgument(CommunicationParameters::messageTimestamp, messageTimestamp) &&
        response.setResponseArgument(CommunicationParameters::subscribers, subscribers))
    {
        // Set status code to success
        response.setStatusCode(Statuscode::success);
    }
    else
    {
        // Set status code to internal server error
        response.setStatusCode(Statuscode::internalError);
    }
    return response;
}

Response Server::handlePublishRequest(string topicName, string message)
{
    // Create Response object
    Response response = Response(CommandIdentifiers::publish);

    // init pointer to topic
    Topic *topicPtr = nullptr;

    // Find the topic with the given name
    for (Topic *topic : this->topics)
    {
        if (topic->getTopicName() == topicName)
        {
            topicPtr = topic;
            break;
        }
    }

    // Check if the topic was found
    if (topicPtr == nullptr)
    {
        // Set status code to failed
        response.setStatusCode(Statuscode::failed);
        return response;
    }

    // Set the message
    // Publishing is done by the Thread of the Server
    if (topicPtr->setMessage(message))
    {
        // Set status code to success
        response.setStatusCode(Statuscode::success);
    }
    else
    {
        // Set status code to failed
        response.setStatusCode(Statuscode::failed);
    }
    return response;
    
}
