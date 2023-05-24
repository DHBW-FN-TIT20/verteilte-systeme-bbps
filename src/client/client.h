#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <ctime>
#include <thread>

#include "../shared/statuscode.h"

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
        
        /**
         * @brief Prints a message with a timestamp.
         *
         * @param message The message to print.
         * @param timestamp The timestamp of the message.
         */
        void printMessage(string message, time_t timestamp);

    private:
        int port;
        string serverAddress;
        int serverPort;
        thread messageThread;
};

#endif