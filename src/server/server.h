#ifndef SERVER_H
#define SERVER_H

#include <ctime>
#include <thread>
#include <list>

#include "topic.h"
#include "clientconnection.h"

using namespace std;

/**
 * @brief Server class.
 * Publisher.
 */
class Server {
    private:
        int port;
        int topicTimeout;
        list<Topic> topics;
        list<ClientConnection> clientConnections;
        thread timeoutCheckerThread;

    public:
        /**
         * @brief Construct a new Server object
         * Creates and starts the \p timeoutCheckerThread thread.
         * @param topicTimeout Timeout in seconds.
         */
        Server(int topicTimeout);

        /**
         * @brief Destroy the Server object
         * Terminates the \p timeoutCheckerThread thread.
         * Removes all Topic and ClientConnection objects.
         */
        ~Server();

        /**
         * @brief Topic timeout checker function.
         * Checks for timed out topics and initiates send of last message, as well as resets the timeout.
         */
        void timeoutCheck();
        /**
         * @brief Removes topics without subscribers.
         * Checks topics for subscribers and removes the topic if there are none.
         */
        void cleanTopics();
        /**
         * @brief Deletes the provided connection if the requirements are met.
         * Checks clientConnections for subscribed topics and removes them if there are no subscribed topics.
         * @param clientConnection Connection to check.
         */
        void cleanClientConnection(ClientConnection clientConnection);
        /**
         * @brief Awaits incoming requests and handles them.
         * Awaits incoming requests and redirects them to the appropriate topic and function.
         */
        void handleRequest();
        /**
         * @brief Log incoming request details.
         * Logs the incoming request details to the console.
         * Includes the source address and port, the message and a timestamp of the request.
         * @param request Incoming request.
         * @param sourceAddress IPv4 Address of the sender.
         * @param sourcePort Port of the sender.
         */
        void logRequest(string request, string sourceAddress, int sourcePort);
};

#endif