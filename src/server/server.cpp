#include <spdlog/spdlog.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <algorithm>

#include "server.h"
#include "../shared/command.h"
#include "../shared/statuscode.h"

#define BUFFER_SIZE 2048

using namespace std;

void Server::handleApproachingClient(int clientSocket, struct sockaddr_in *clientAddress)
{
    char buffer[BUFFER_SIZE];

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0)
    {
        spdlog::error("Error receiving data from client");
        close(clientSocket);
        return;
    }
    // Process the received data
    string receivedData(buffer, bytesRead);
    spdlog::debug("Received from client: {}", receivedData);

    // Get the ip address and port of the client
    struct sockaddr_in incomingAddr;
    socklen_t incomingAddrLen = sizeof(incomingAddr);
    getpeername(clientSocket, (struct sockaddr *)&incomingAddr, &incomingAddrLen);

    string clientIpAddress = inet_ntoa(incomingAddr.sin_addr);
    spdlog::info("Received request from {}:{} with content: {}", clientIpAddress, ntohs(incomingAddr.sin_port), receivedData);

    // deserialize the received data
    // try catch to catch the exception if the string is not a valid json
    Command command;
    Response response;
    try
    {
        command = Command::deserialize(receivedData);

        switch (command.commandIdentifier)
        {
        case CommandIdentifiers::subscribe:
            response = this->handleSubscribeRequest(clientIpAddress, stoi(command.getCommandArgument(CommunicationParameters::clientPort)), command.getCommandArgument(CommunicationParameters::topicName));
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
    }
    catch (const exception &e)
    {
        spdlog::error("The request is not from type JSON (Error: {})", e.what());

        response = Response();
        response.setStatusCode(Statuscode::internalError);
    }

    // serialize the response
    string serializedResponse = response.serialize();

    // send the response to the client
    int bytesSent = send(clientSocket, serializedResponse.c_str(), serializedResponse.length(), 0);
    if (bytesSent != serializedResponse.length())
    {
        spdlog::error("Error sending data to client");
        close(clientSocket);
        return;
    }

    // close the connection
    close(clientSocket);
}

void Server::startServer(int port)
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        spdlog::error("Error creating socket");
        return;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        spdlog::error("Error binding socket");
        return;
    }

    int backlog = 5; // Maximum number of pending connections
    if (listen(serverSocket, backlog) < 0)
    {
        spdlog::error("Error listening for connections");
        return;
    }

    vector<thread> clientThreads;

    while (true)
    {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSocket < 0)
        {
            spdlog::error("Error accepting connection");
            continue;
        }

        // Create a new thread to handle the approaching client connection
        thread threadObj(&Server::handleApproachingClient, this, clientSocket, &clientAddress);
        clientThreads.push_back(move(threadObj));
    }
}

void Server::checkTopicTimeouts()
{
    // For all Topics check if the timeout has been reached
    while (true)
    {
        mtx.lock();
        for (Topic *topic : this->topics)
        {
            // if message timeout < current time
            if (topic->getTimeoutTimestamp() <= time(nullptr))
            {
                // publish message
                topic->publishMessage();
                // update timeout timestamp
                topic->setTimeoutTimestamp(time(nullptr) + this->topicTimeout);
            }
        }
        mtx.unlock();
        // Sleep for 1 second
        sleep(1);
    }
}

Server::Server(int port, int topicTimeout)
{
    spdlog::info("Starting server on port {} with topic timeout of {} seconds.", port, topicTimeout);
    this->topicTimeout = topicTimeout;
    this->timeoutCheckerThread = thread(&Server::checkTopicTimeouts, this);
    this->clientConnections = {};
    this->topics = {};
    // Start the Server in a while loop
    startServer(port);
}

Server::~Server()
{
    this->timeoutCheckerThread.join();
    spdlog::info("Stopping server");
}

Response Server::handleSubscribeRequest(string ipAddress, int port, string topicName)
{
    // Create Response object
    Response response = Response(CommandIdentifiers::subscribe);
    // lock the mutex
    lock_guard<mutex> lock(mtx);
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

    // if the topic does not exist create a new topic
    if (topicPtr == nullptr)
    {
        // Create the Topic
        topicPtr = new Topic(topicName);
        // Add the topic to the list of topics
        this->topics.push_back(topicPtr);
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

    if (clientConnectionPtr == nullptr)
    {
        // Create the ClientConnection
        clientConnectionPtr = new ClientConnection(ipAddress, port);
        // Add the ClientConnection to the list of ClientConnections
        this->clientConnections.push_back(clientConnectionPtr);
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
    // lock the mutex
    lock_guard<mutex> lock(mtx);
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
    // if the topic has no more clients, delete the topic
    if ((topicPtr->getClientConnections()).size() == 0)
    {
        topics.erase(remove(topics.begin(), topics.end(), topicPtr), topics.end());
        delete topicPtr;
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
        clientConnections.erase(remove(clientConnections.begin(), clientConnections.end(), clientConnectionPtr), clientConnections.end());
        delete clientConnectionPtr;
    }
    return response;
}

Response Server::handleListTopics()
{
    // Create Response object
    Response response = Response(CommandIdentifiers::listTopics);

    // lock the mutex
    lock_guard<mutex> lock(mtx);

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

    // lock the mutex
    lock_guard<mutex> lock(mtx);

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
        // Set status code to invalid parameter
        response.setStatusCode(Statuscode::invalidParameter);
        return response;
    }

    // get message timestamp
    time_t messageTimestamp = topicPtr->getMessageTimestamp();

    // get list of Subscribers as vector of ClientConnections
    vector<ClientConnection *> connections = topicPtr->getClientConnections();

    // cast the IP Adresses in a vector of strings
    vector<string> subscribers;
    for (ClientConnection *connection : connections)
    {
        subscribers.push_back(connection->getAddress() + ":" + to_string(connection->getPort()));
    }

    // Set response Arguments
    if (response.setResponseArgument(CommunicationParameters::messageTimestamp, static_cast<uint64_t>(messageTimestamp)) &&
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

    // lock the mutex
    lock_guard<mutex> lock(mtx);

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
        // Set status code to invalid parameter
        response.setStatusCode(Statuscode::invalidParameter);
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
