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
#include "../shared/command.h"
#include "../shared/response.h"
#include "../shared/message.h"
#include "../shared/statuscode.h"
#include "../shared/commandIdentifiers.h"
#include "../shared/communicationParameters.h"
#include "../shared/arrayStringToVectorString.h"

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
        // Set the port to the actually used port
        this->port = ntohs(addr.sin_port);
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

    Command subscribe(CommandIdentifiers::subscribe);

    subscribe.setCommandArgument(CommunicationParameters::topicName, topicName);
    subscribe.setCommandArgument(CommunicationParameters::clientPort, to_string(this->port));

    sendMessage(subscribe);
    
    spdlog::debug("--------------------");
}

void Client::unsubscribe(string topicName) {
    spdlog::debug("--------------------");
    spdlog::debug("Unsubscribing from topic: {}", topicName);

    Command unsubscribe(CommandIdentifiers::unsubscribe);

    unsubscribe.setCommandArgument(CommunicationParameters::topicName, topicName);

    sendMessage(unsubscribe);

    spdlog::debug("--------------------");
}

void Client::publishTopic(string topicName, string message) {
    spdlog::debug("--------------------");
    spdlog::debug("Publishing message: {} to topic: {}", message, topicName);

    Command publish = Command(CommandIdentifiers::publish);

    publish.setCommandArgument(CommunicationParameters::topicName, topicName);
    publish.setCommandArgument(CommunicationParameters::message, message);

    sendMessage(publish);

    spdlog::debug("--------------------");
}

void Client::listTopics() {
    spdlog::debug("--------------------");
    spdlog::debug("Listing topics...");

    Command listTopics = Command(CommandIdentifiers::listTopics);

    sendMessage(listTopics);

    spdlog::debug("--------------------");
}

void Client::getTopicStatus(string topicName) {
    spdlog::debug("--------------------");
    spdlog::debug("Getting status of topic: {}", topicName);

    Command getTopicStatus = Command(CommandIdentifiers::getTopicStatus);

    getTopicStatus.setCommandArgument(CommunicationParameters::topicName, topicName);

    sendMessage(getTopicStatus);
    
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

            try {
                Message message = Message::deserialize(clientMessage);
                printMessage(message);
            } catch (const runtime_error &e) {
                spdlog::error("{}", e.what());
                continue;
            }
        }
    }

    spdlog::debug("Message thread terminated.");
}

void Client::printMessage(Message message) {
    char formattedTimestamp[21];
    const time_t timestamp = message.getMessageTimestamp();
    strftime(formattedTimestamp, sizeof(formattedTimestamp), "%d.%m.%Y %H:%M::%S", localtime(&timestamp));

    spdlog::info("Message received:\nTopic: {}\nTimestamp: {}\nMessage: {}", message.getTopic(), formattedTimestamp, message.getMessage());
}

void Client::sendMessage(Command &command) {
    string message = command.serialize();

    // send message to server
    spdlog::info("Sending message: {}", message);
    // TODO: Check if the complete message is sent
    sendto(serverSocket, (const char *)message.c_str(), strlen(message.c_str()), MSG_CONFIRM, (const struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // prepare response buffer
    char responseJson[BUFFER_SIZE];
    strncpy(responseJson, "", sizeof(responseJson));
    struct sockaddr_in incomingAddr;
    socklen_t incomingAddrLen = sizeof(incomingAddr);

    // receive responseJson from server
    if (recvfrom(serverSocket, responseJson, sizeof(responseJson), 0, (struct sockaddr*)&incomingAddr, &incomingAddrLen) < 0) {
        spdlog::error("No response from the server within 30 seconds.");
    } else {
        spdlog::info("Received message from {}:{}", inet_ntoa(incomingAddr.sin_addr), ntohs(incomingAddr.sin_port));

        // parse response
        try {
            Response response = Response::deserialize(command.commandIdentifier, responseJson);
            spdlog::debug("Response deserialized");
            logResponse(response, inet_ntoa(incomingAddr.sin_addr), to_string(ntohs(incomingAddr.sin_port)));
        } catch (const runtime_error &e) {
            spdlog::error("Response cannot be parsed: {}", e.what());
        }
    }
}

void Client::logResponse(const Response &response, const string &senderIp, const string &senderPort) {
    // Sender: <ip>:<port> - Status: <status> - 
    //  - Topics: <topic1>, <topic2>, <topic3>
    //  - Message Timestamp: <timestamp>, Subscribers: <subscriber1>, <subscriber2>, <subscriber3>

    spdlog::debug("Logging response...");

    string logString = "";
    // Sender Info
    logString += "Sender: " + senderIp + ":" + senderPort + "\n";

    // Status
    logString += "Status: ";
    
    switch (response.getStatusCode()) {
        case Statuscode::success:
            logString += "Success";
            break;

        case Statuscode::failed:
            logString += "Failed";
            break;

        case Statuscode::invalidParameter:
            logString += "Invalid Parameter";
            break;

        case Statuscode::internalError:
            logString += "Internal Error";
            break;

        default:
            logString += "Unknown status code: " + to_string(static_cast<int>(response.getStatusCode()));
            break;
    }

    logString += "\n";

    // Response Arguments
    vector<CommunicationParameters> parameters = response.getAvailableArguments();

    for (CommunicationParameters parameter : parameters) {
        switch (parameter) {
        case CommunicationParameters::topics:
            {
                vector<string> topics = arrayStringToVectorString(response.getResponseArgument(parameter));
                logString += "Topics: ";
                for (string topic : topics) {
                    logString += topic;
                    if (topic != topics.back()) {
                        logString += ", ";
                    }
                }
                logString += "\n";
                break;
            }
        case CommunicationParameters::messageTimestamp:
            {
                time_t timestamp = static_cast<time_t>(stoll(response.getResponseArgument(parameter)));
                char formattedTimestamp[21];
                strftime(formattedTimestamp, sizeof(formattedTimestamp), "%d.%m.%Y %H:%M::%S", localtime(&timestamp));

                logString += "Message Timestamp: " + string(formattedTimestamp) +"\n";
                break;
            }
        case CommunicationParameters::subscribers:
            {
                vector<string> subscribers = arrayStringToVectorString(response.getResponseArgument(parameter));
                logString += "Subscribers: ";
                for (string subscriber : subscribers) {
                    logString += subscriber;
                    if (subscriber != subscribers.back()) {
                        logString += ", ";
                    }
                }
                logString += "\n";
                break;
            }
        default:
            logString += communicationParameterToStringDictionary[parameter] + ": " + response.getResponseArgument(parameter) + "\n";
            break;
        }
    }

    spdlog::info(logString);
    spdlog::debug("Response logged.");
}
