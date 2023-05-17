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
         * Starts a new thread (\p messageThread) that is listening for incoming messages on the defined port.
         * @param port Set 0 for automatic port assignment
         * @param serverAddress IPv4 Address of the server
         * @param serverPort Port number of the server
         */
        Client(int port, string serverAddress, int serverPort);

        /**
         * @brief Destroy the Client object
         * Terminates the \p messageThread.
         */
        ~Client();
        
        
        /**
         * @brief Subscribes to a topic.
         * Sends a subscribe request to the server.
         * @param topicName The name of the topic to subscribe to.
         */
        void subscribeTopic(string topicName);

        /**
         * @brief Unsubscribes from a topic.
         * Sends a unsubscribe request to the server.
         * @param topicName The name of the topic to unsubscribe from.
         */
        void unsubscribe(string topicName);

        /**
         * @brief Publishes a message to a topic.
         * Sends a publish request to a specific topicName with an message
         * @param topicName The name of the topic to publish the message to.
         * @param message The message to publish.
         */
        void publishTopic(string topicName, string message);

        /**
         * @brief Lists all available topics.
         * Anforderung: Fordert eine Liste der verfügbaren Topics an.
         * Parameter: Keine
         * Rückgabe: Liste der Verfügbaren Topics.
                     Erfolg oder Misserfolg:
                     Topics erfolgreich übermittelt
                     Ungültige Parameter
                     Weitere interne Fehler
         */
        void listTopic();

        /**
         * @brief Retrieves the status of a topic.
         *
         * @param topicName The name of the topic to retrieve the status of.
         */
        void getTopicStatus(string topicName);

        /**
         * @brief Handles incoming messages.
         *
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