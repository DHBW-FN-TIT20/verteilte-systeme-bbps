#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <ctime>
#include <thread>
#include <netinet/in.h>
#include <jsoncpp/json/json.h>

#include "../shared/command.h"
#include "../shared/statuscode.h"
#include "../shared/response.h"
#include "../shared/message.h"

using namespace std;

class Client {
    public:
        /**
         * @brief Construct a new Client object
         * 
         * @param port Set 0 for automatic port assignment
         * @param serverAddress IPv4 Address of the server
         * @param serverPort Port number of the server
         * 
         * @throws runtime_error if the port is already in use
         * 
         * @details
         * Starts a new thread (\p messageThread) that is listening for incoming messages on the defined port.
         * Since the connection to the server is connectionless (UDP), errors due to serverAddress or serverPort will occur when sending a message.
         */
        Client(int port, string serverAddress, int serverPort);

        /**
         * @brief Destroy the Client object
         * 
         * @details
         * Terminates the \p messageThread.
         */
        ~Client();
        
        /**
         * @brief Subscribes to a topic.
         * 
         * @param topicName The name of the topic to subscribe to.
         * 
         * @details
         * Sends a subscribe request to the server.
         */
        void subscribeTopic(string topicName);

        /**
         * @brief Unsubscribes from a topic.
         * 
         * @param topicName The name of the topic to unsubscribe from.
         * 
         * @details
         * Sends a unsubscribe request to the server.
         */
        void unsubscribe(string topicName);

        /**
         * @brief Publishes a message to a topic.
         * 
         * @param topicName The name of the topic to publish the message to.
         * @param message The message to publish.
         * 
         * @details
         * Sends a publish request to a specific topicName with an message
         */
        void publishTopic(string topicName, string message);

        /**
         * @brief Lists all available topics.
         * 
         * @details
         * Sends a list request to the server.
         * Receives and prints a list of all available topics.
         */
        void listTopics();

        /**
         * @brief Retrieves the status of a topic.
         *
         * @param topicName The name of the topic to retrieve the status of.
         * 
         * @details
         * Sends a getTopic request to the server.
         * Receives the timestamp of the last message and prints it.
         */
        void getTopicStatus(string topicName);

        /**
         * @brief Handles incoming messages.
         *
         * @details
         * This method runs on a separate thread and continuously listens for incoming messages
         * from the server.
         */
        void handleMessages();
        
    private:
        int port;
        string serverAddress;
        int serverPort;
        int serverSocket;
        struct sockaddr_in serverAddr;
        int messageSocket;
        struct sockaddr_in clientAddr;
        // Atomic bool does not need a mutex
        atomic<bool> messageThreadRunning;
        thread messageThread;

        /**
         * @brief Sends a given command to the server.
         * 
         * @param command Object specifying the parameters to send.
         */
        void sendMessage(Command &command);
        /**
         * @brief Prints the information of a received response to the console.
         * 
         * @param response Object containing the response information.
         * @param senderIp IP of the sender (server).
         * @param senderPort Port of the sender (server).
         */
        void logResponse(const Response &response, const string &senderIp, const string &senderPort);
        /**
         * @brief Prints a message to a topic with a timestamp to the console.
         *
         * @param message The message object containing all informations.
         */
        void printMessage(Message message);
};

#endif