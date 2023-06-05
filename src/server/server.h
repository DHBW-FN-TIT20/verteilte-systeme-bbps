#ifndef SERVER_H
#define SERVER_H

#include <ctime>
#include <thread>
#include <list>

#include "topic.h"
#include "clientconnection.h"

using namespace std;

/**
 * @brief Publisher
 */
class Server {
    public:
        /**
         * @brief Construct a new Server object
         * 
         * @param topicTimeout Timeout in seconds.
         * @param port Port to listen on.
         * @details
         * Creates and starts the \p timeoutCheckerThread thread.
         * Logs the start of the server including the server address and port.
         */
        Server(int port, int topicTimeout);

        /**
         * @brief Destroy the Server object
         * 
         * @details
         * Terminates the \p timeoutCheckerThread thread.
         * Removes all Topic and ClientConnection objects.
         */
        ~Server();


        /**
         * @brief Topic timeout checker function.
         * 
         * @details
         * Checks for timed out topics and initiates send of last message, as well as resets the timeout.
         */
        void timeoutCheck();

        /**
         * @brief Removes topics without subscribers.
         * 
         * @details
         * Checks topics for subscribers and removes the topic if there are none.
         */
        void cleanTopics();

        /**
         * @brief Deletes the provided connection if the requirements are met.
         * 
         * @param clientConnection Connection to check.
         * 
         * @details 
         * Checks clientConnections for subscribed topics and removes them if there are no subscribed topics.
         */
        void cleanClientConnection(ClientConnection clientConnection);
        
        /**
         * @brief Awaits incoming requests and handles them.
         * 
         * @details 
         * Awaits incoming requests and redirects them to the appropriate topic and function.
         * Sends a response to the sender.
         * Example:
         * 
         * @note Requirements:
         * @note - Check the message for missing/invalid parameters.
         * @note - If a topic does not exits, a new one will be created.
         */
        void handleRequest();
        
        /**
         * @brief Log incoming request details.
         * 
         * @param request Incoming request.
         * @param sourceAddress IPv4 Address of the sender.
         * @param sourcePort Port of the sender.
         * 
         * @details
         * Logs the incoming request details to the console.
         * Includes the source address and port, the message and a timestamp of the request.
         * 
         * @note Requirements:
         * @note - The timestamp must be displayed in a readable format. (z.B. TT.MM.JJJJ HH:MM:SS)
         */
        void logRequest(string request, string sourceAddress, int sourcePort);

    private:
        int port;
        int topicTimeout;
        list<Topic> topics;
        list<ClientConnection> clientConnections;
        thread timeoutCheckerThread;
        Response handleSubsscribeRequest(int socket, string topicName);
        Response handleUnsubscribeRequest(int socket, string topicName);
        Response handleListTopics();
        Response handleGetTopicStatus(string topicName);
};

#endif