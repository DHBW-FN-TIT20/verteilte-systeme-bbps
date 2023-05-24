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
    spdlog::info("Message thread terminated.");
}

void Client::subscribeTopic(string topicName) {
    spdlog::info("Subscribing to topic: {}", topicName);

    Json::Value root;
    root["command"] = "subscribe";
    root["topic"] = topicName;

    cout << root << endl;

    // sendto(serverSocket, (const char *)DATA, strlen(DATA), MSG_CONFIRM, (const struct sockaddr *)&serverAddr, sizeof(serverAddr));
}

void Client::unsubscribe(string topicName) {
    spdlog::info("Unsubscribing from topic: {}", topicName);
}

void Client::publishTopic(string topicName, string message) {
    spdlog::info("Publishing message: {} to topic: {}", message, topicName);
}

void Client::listTopics() {
    spdlog::info("Listing topics...");
}

void Client::getTopicStatus(string topicName) {
    spdlog::info("Getting status of topic: {}", topicName);
}

void Client::handleMessages() {
    spdlog::info("Message thread started.");

    struct sockaddr_in incomingAddr;
    socklen_t incomingAddrLen = sizeof(incomingAddr);
    char clientMessage[2000];

    while (this->messageThreadRunning) {
        strncpy(clientMessage, "", sizeof(clientMessage));

        if (recvfrom(messageSocket, clientMessage, sizeof(clientMessage), 0, (struct sockaddr*)&incomingAddr, &incomingAddrLen) < 0) {
            spdlog::error("message receive failed");
        } else {
            spdlog::info("Received message from {}:{}", inet_ntoa(incomingAddr.sin_addr), ntohs(incomingAddr.sin_port));
        }

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