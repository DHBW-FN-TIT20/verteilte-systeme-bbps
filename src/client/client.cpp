// TODO: Global: make sure that receive buffer is big enough for all messages
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <jsoncpp/json/json.h>

#include "client.h"

using namespace std;

Client::Client(int port, string serverAddress, int serverPort) {
    spdlog::info("Initializing client with port {}, server address {} and server port {}...", port, serverAddress, serverPort);
    this->port = port;
    this->serverAddress = serverAddress;
    this->serverPort = serverPort;

    // 
    // Creating a socket for UDP communication with the server 
    // 

    // Creating socket file descriptor
    spdlog::info("Creating server connection...");
    if ( (serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        spdlog::error("server socket creation failed");
        throw runtime_error("server socket creation failed");
    }
   
    memset(&serverAddr, 0, sizeof(serverAddr));
       
    // Filling server information
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverAddress.c_str());
    spdlog::info("Server connection created.");

    // Setting a timeout for the response from the server.
    struct timeval tvServer;
    tvServer.tv_sec = 30;
    tvServer.tv_usec = 0;
    setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tvServer, sizeof tvServer);


    // 
    // Creating a socket for receiving messages from the server and starting the message thread
    // 

    spdlog::info("Creating message socket...");
    if ( (messageSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        spdlog::error("message socket creation failed");
        throw runtime_error("message socket creation failed");
    }

    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(port);
    clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Setting a timeout for the message socket -> recv will return after 5 seconds if no message has been received
    // This is necessary to be able to check if the message thread should be terminated, otherwise it would block forever
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(messageSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    if (bind(messageSocket, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) == -1) {
        spdlog::error("message socket bind failed");
        throw runtime_error("message socket bind failed");
    } else {
        struct sockaddr_in addr;
        socklen_t addr_size = sizeof(addr);
        getsockname(messageSocket, (struct sockaddr *) &addr, &addr_size);
        spdlog::info("Successfully bound message socket to {}:{}.", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }

    // starting the message thread
    spdlog::info("Starting message thread...");
    this->messageThreadRunning = true;
    messageThread = thread{&Client::handleMessages, this}; 
}

Client::~Client() {
    spdlog::info("Terminating client...");

    spdlog::info("Terminating message thread...");
    // TODO: Aktuell wird der Thread erst beendet nachdem eine neue Nachricht gekommen ist da er sonst im recv festhängt -> eventuell doch einfach killen.
    this->messageThreadRunning = false;
    messageThread.join();
    spdlog::info("Client terminated.");
}

void Client::subscribeTopic(string topicName) {
    spdlog::info("--------------------");
    spdlog::info("Subscribing to topic: {}", topicName);

    // TODO: Check correct syntax for json request
    Json::Value root;
    root["command"] = "subscribe";
    root["topic"] = topicName;

    Json::Value response = sendMessage(root);

    // TODO: Check correct syntax for json response
    // TODO: Do sth. with response (according to requirements)
    
    spdlog::info("--------------------");
}

void Client::unsubscribe(string topicName) {
    spdlog::info("--------------------");
    spdlog::info("Unsubscribing from topic: {}", topicName);

    // TODO: Check correct syntax for json request
    Json::Value root;
    root["command"] = "unsubscribe";
    root["topic"] = topicName;

    Json::Value response = sendMessage(root);

    // TODO: Check correct syntax for json response
    // TODO: Do sth. with response (according to requirements)

    spdlog::info("--------------------");
}

void Client::publishTopic(string topicName, string message) {
    spdlog::info("--------------------");
    spdlog::info("Publishing message: {} to topic: {}", message, topicName);

    // TODO: Check correct syntax for json request
    Json::Value root;
    root["command"] = "publish";
    root["topic"] = topicName;
    root["message"] = message;

    Json::Value response = sendMessage(root);

    // TODO: Check correct syntax for json response
    // TODO: Do sth. with response (according to requirements)

    spdlog::info("--------------------");
}

void Client::listTopics() {
    spdlog::info("--------------------");
    spdlog::info("Listing topics...");

    // TODO: Check correct syntax for json request
    Json::Value root;
    root["command"] = "listTopics";

    Json::Value response = sendMessage(root);

    // TODO: Check correct syntax for json response
    // TODO: Do sth. with response (according to requirements)

    spdlog::info("--------------------");
}

void Client::getTopicStatus(string topicName) {
    spdlog::info("--------------------");
    spdlog::info("Getting status of topic: {}", topicName);

    // TODO: Check correct syntax for json request
    Json::Value root;
    root["command"] = "getTopicStatus";
    root["topic"] = topicName;

    Json::Value response = sendMessage(root);

    // TODO: Check correct syntax for json response
    // TODO: Do sth. with response (according to requirements)
    
    spdlog::info("--------------------");
}

void Client::handleMessages() {
    spdlog::info("Message thread started.");

    struct sockaddr_in incomingAddr;
    socklen_t incomingAddrLen = sizeof(incomingAddr);
    char clientMessage[2000];

    while (this->messageThreadRunning) {
        strncpy(clientMessage, "", sizeof(clientMessage));

        if (recvfrom(messageSocket, clientMessage, sizeof(clientMessage), 0, (struct sockaddr*)&incomingAddr, &incomingAddrLen) < 0) {
            spdlog::info("No message received within timeout.");
        } else {
            spdlog::info("Received message from {}:{}", inet_ntoa(incomingAddr.sin_addr), ntohs(incomingAddr.sin_port));

            Json::CharReaderBuilder builder {};
            auto reader = unique_ptr<Json::CharReader>(builder.newCharReader());

            Json::Value root {};
            string errors {};
            const auto is_parsed = reader->parse( clientMessage, clientMessage + 2000, &root, &errors );

            if (!is_parsed) {
                spdlog::error("message parsing failed with error: {}", errors);
            } else {
                Json::StreamWriterBuilder writer;
                spdlog::info("message received: \n{}", Json::writeString(writer, root));
            }
        }
    }

    spdlog::info("Message thread terminated.");
}

Json::Value Client::sendMessage(const Json::Value &data) {
    // convert json to sendable char array
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    string message = Json::writeString(writer, data);

    // send message to server
    spdlog::info("Sending message: {}", message);
    // TODO: Check if the complete message is sent
    sendto(serverSocket, (const char *)message.c_str(), strlen(message.c_str()), MSG_CONFIRM, (const struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // prepare response buffer
    char response[2000];
    strncpy(response, "", sizeof(response));
    struct sockaddr_in incomingAddr;
    socklen_t incomingAddrLen = sizeof(incomingAddr);
    Json::Value responseJson {};

    // receive response from server
    if (recvfrom(serverSocket, response, sizeof(response), 0, (struct sockaddr*)&incomingAddr, &incomingAddrLen) < 0) {
        spdlog::error("No response from the server within 30 seconds.");
    } else {
        spdlog::info("Received message from {}:{}", inet_ntoa(incomingAddr.sin_addr), ntohs(incomingAddr.sin_port));

        // parse response
        Json::CharReaderBuilder builder {};
        auto reader = unique_ptr<Json::CharReader>(builder.newCharReader());

        string errors {};
        const auto is_parsed = reader->parse( response, response + sizeof(response), &responseJson, &errors );

        if (!is_parsed) {
            spdlog::error("Message parsing failed with error: {}", errors);
        } else {
            Json::StreamWriterBuilder writer;
            spdlog::info("Message received: \n{}", Json::writeString(writer, responseJson));
        }
    }

    return responseJson;
}