// TODO: Global: make sure that receive buffer is big enough for all messages
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <string>

#include "client.h"

#define SERVER_RESPONSE_TIMEOUT 30
#define BUFFER_SIZE 2048

using namespace std;

Client::Client(int port, string serverAddress, int serverPort) {
    spdlog::debug("Initializing client with port {}, server address {} and server port {}...", port, serverAddress, serverPort);
    this->port = port;
    this->serverAddress = serverAddress;
    this->serverPort = serverPort;

    // 
    // Creating a socket for UDP communication with the server 
    // 
    spdlog::debug("Creating server connection...");

    // Creating socket file descriptor
    if ( (serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        spdlog::error("Server socket creation failed");
        throw runtime_error("Server socket creation failed");
    }
   
    memset(&serverAddr, 0, sizeof(serverAddr));
       
    // Filling server information
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverAddress.c_str());

    // Setting a timeout for the response from the server.
    struct timeval tvServer;
    tvServer.tv_sec = SERVER_RESPONSE_TIMEOUT;
    tvServer.tv_usec = 0;
    setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tvServer, sizeof tvServer);

    spdlog::debug("Server connection created.");

    // 
    // Creating a socket for receiving messages from the server and starting the message thread
    // 

    spdlog::debug("Creating message socket...");
    if ( (messageSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        spdlog::error("Message socket creation failed");
        throw runtime_error("Message socket creation failed");
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
        spdlog::error("Message socket bind failed");
        throw runtime_error("Message socket bind failed");
    } else {
        struct sockaddr_in addr;
        socklen_t addr_size = sizeof(addr);
        getsockname(messageSocket, (struct sockaddr *) &addr, &addr_size);
        spdlog::info("Successfully bound message socket to {}:{}.", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }

    // starting the message thread
    spdlog::debug("Starting message thread...");
    this->messageThreadRunning = true;
    messageThread = thread{&Client::handleMessages, this}; 
}

Client::~Client() {
    spdlog::debug("Terminating client...");

    spdlog::debug("Terminating message thread...");
    // TODO: Aktuell wird der Thread erst beendet nachdem eine neue Nachricht gekommen ist da er sonst im recv festhängt -> eventuell doch einfach killen.
    this->messageThreadRunning = false;
    messageThread.join();
    spdlog::debug("Client terminated.");
}

void Client::subscribeTopic(string topicName) {
    spdlog::debug("--------------------");
    spdlog::debug("Subscribing to topic: {}", topicName);

    // TODO: Check correct syntax for json request
    Json::Value root;
    root["command"] = "subscribe";

    Json::Value arguments(Json::arrayValue);
    arguments.append(topicName);
    root["arguments"] = arguments;

    Json::Value response = sendMessage(root);

    logResponse(response);

    // TODO: Check correct syntax for json response
    // TODO: Do sth. with response (according to requirements)
    
    spdlog::debug("--------------------");
}

void Client::unsubscribe(string topicName) {
    spdlog::debug("--------------------");
    spdlog::debug("Unsubscribing from topic: {}", topicName);

    // TODO: Check correct syntax for json request
    Json::Value root;
    root["command"] = "unsubscribe";

    Json::Value arguments(Json::arrayValue);
    arguments.append(topicName);
    root["arguments"] = arguments;

    Json::Value response = sendMessage(root);

    // TODO: Check correct syntax for json response
    // TODO: Do sth. with response (according to requirements)

    spdlog::debug("--------------------");
}

void Client::publishTopic(string topicName, string message) {
    spdlog::debug("--------------------");
    spdlog::debug("Publishing message: {} to topic: {}", message, topicName);

    // TODO: Check correct syntax for json request
    Json::Value root;
    root["command"] = "publish";

    Json::Value arguments(Json::arrayValue);
    arguments.append(topicName);
    arguments.append(message);
    root["arguments"] = arguments;

    Json::Value response = sendMessage(root);

    // TODO: Check correct syntax for json response
    // TODO: Do sth. with response (according to requirements)

    spdlog::debug("--------------------");
}

void Client::listTopics() {
    spdlog::debug("--------------------");
    spdlog::debug("Listing topics...");

    // TODO: Check correct syntax for json request
    Json::Value root;
    root["command"] = "listTopics";

    Json::Value arguments(Json::arrayValue);
    root["arguments"] = arguments;

    Json::Value response = sendMessage(root);

    // TODO: Check correct syntax for json response
    // TODO: Do sth. with response (according to requirements)

    spdlog::debug("--------------------");
}

void Client::getTopicStatus(string topicName) {
    spdlog::debug("--------------------");
    spdlog::debug("Getting status of topic: {}", topicName);

    // TODO: Check correct syntax for json request
    Json::Value root;
    root["command"] = "getTopicStatus";

    Json::Value arguments(Json::arrayValue);
    arguments.append(topicName);
    root["arguments"] = arguments;

    Json::Value response = sendMessage(root);

    // TODO: Check correct syntax for json response
    // TODO: Do sth. with response (according to requirements)
    
    spdlog::debug("--------------------");
}

void Client::handleMessages() {
    spdlog::debug("Message thread started.");

    struct sockaddr_in incomingAddr;
    socklen_t incomingAddrLen = sizeof(incomingAddr);
    char clientMessage[BUFFER_SIZE];

    while (this->messageThreadRunning) {
        strncpy(clientMessage, "", sizeof(clientMessage));

        if (recvfrom(messageSocket, clientMessage, sizeof(clientMessage), 0, (struct sockaddr*)&incomingAddr, &incomingAddrLen) < 0) {
            spdlog::debug("No message received within timeout.");
        } else {
            spdlog::info("Received message from {}:{}", inet_ntoa(incomingAddr.sin_addr), ntohs(incomingAddr.sin_port));

            Json::CharReaderBuilder builder {};
            auto reader = unique_ptr<Json::CharReader>(builder.newCharReader());

            Json::Value root {};
            string errors {};
            const auto is_parsed = reader->parse( clientMessage, clientMessage + 2000, &root, &errors );

            if (!is_parsed) {
                spdlog::error("Message parsing failed with error: {}", errors);
            } else {
                Json::StreamWriterBuilder writer;
                spdlog::debug("Message received: \n{}", Json::writeString(writer, root));

                if (root.isMember("topic") && root.isMember("messageTimestamp") && root.isMember("message")) {
                    string topic = root["topic"].asString();
                    string message = root["message"].asString();
                    time_t timestamp = static_cast<time_t>(stoll(root["messageTimestamp"].asString()));

                    printMessage(topic, message, timestamp);
                } else {
                    spdlog::error("Message does not contain all required fields.");
                }
            }
        }
    }

    spdlog::debug("Message thread terminated.");
}

void Client::printMessage(string topic, string message, time_t timestamp) {
    char formattedTimestamp[21];
    strftime(formattedTimestamp, sizeof(formattedTimestamp), "%d.%m.%Y %H:%M::%S", localtime(&timestamp));

    spdlog::info("\nTopic: {}\nTimestamp: {}\nMessage: {}", topic, formattedTimestamp, message);
}

Json::Value Client::sendMessage(const Json::Value &data) {
    // convert json to sendable char array
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    string message = Json::writeString(writer, data);

    // send message to server
    spdlog::debug("Sending message: {}", message);
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
        spdlog::debug("Received message from {}:{}", inet_ntoa(incomingAddr.sin_addr), ntohs(incomingAddr.sin_port));

        // parse response
        Json::CharReaderBuilder builder {};
        auto reader = unique_ptr<Json::CharReader>(builder.newCharReader());

        string errors {};
        const auto is_parsed = reader->parse( response, response + sizeof(response), &responseJson, &errors );

        if (!is_parsed) {
            spdlog::error("Message parsing failed with error: {}", errors);
        } else {
            Json::StreamWriterBuilder writer;
            spdlog::debug("Message received: \n{}", Json::writeString(writer, responseJson));

            // append sender ip and port to response
            responseJson["senderIp"] = inet_ntoa(incomingAddr.sin_addr);
            responseJson["senderPort"] = ntohs(incomingAddr.sin_port);
        }
    }

    return responseJson;
}

void Client::logResponse(const Json::Value &response) {
    // Sender: <ip>:<port> - Status: <status> - 
    //  - Topics: <topic1>, <topic2>, <topic3>
    //  - Message Timestamp: <timestamp>, Subscribers: <subscriber1>, <subscriber2>, <subscriber3>

    string logString = "";
    
    if (response.isMember("senderIp") && response.isMember("senderPort")) {
        logString += "Sender: " + response["senderIp"].asString() + ":" + response["senderPort"].asString() + " - ";
    }
    if (response.isMember("statusCode")) {
        // TODO: Replace status code with text
        logString += "Status: " + response["statusCode"].asString() + " - ";
    }
    if (response.isMember("topics")) {
        logString += "Topics: ";
        for (auto topic : response["topics"]) {
            logString += topic.asString() + ", ";
        }
        logString += " - ";
    }
    if (response.isMember("messageTimestamp")) {
        time_t timestamp = static_cast<time_t>(stoll(response["messageTimestamp"].asString()));
        char formattedTimestamp[21];
        strftime(formattedTimestamp, sizeof(formattedTimestamp), "%d.%m.%Y %H:%M::%S", localtime(&timestamp));

        logString += "Message Timestamp: " + string(formattedTimestamp) +" - ";
    }
    if (response.isMember("subscribers")) {
        logString += "Subscribers: ";
        for (auto subscriber : response["subscribers"]) {
            logString += subscriber.asString() + ", ";
        }
    }

    spdlog::info(logString);
}
