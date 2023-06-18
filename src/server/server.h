#ifndef SERVER_H
#define SERVER_H

#include <ctime>
#include <thread>
#include <vector>
#include <mutex>

#include "topic.h"
#include "clientconnection.h"
#include "../shared/response.h"

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
         * Terminates the \p timeoutSendMessageCheckerThread thread.
         * Removes all Topic and ClientConnection objects.
         */
        ~Server();
        
    private:
        mutex mtx;
        int port;
        int topicTimeout;
        vector<Topic*> topics;
        vector<ClientConnection*> clientConnections;
        thread timeoutCheckerThread;

        /**
         * @brief Starts the server
         * @details Opens the Server Socket and handles approaching clients.
         *
         * @param port 
         */
        void startServer(int port);

        /**
         * @brief Checks for timed out topics and initiates send of last message, as well as resets the timeout.
         * 
         */
        void checkTopicTimeouts();

        /**
         * @brief Handles a approaching client.
         * 
         * @param clientSocket client socket
         * @param clientAddress client address structure
         */
        void handleApproachingClient(int clientSocket, struct sockaddr_in* clientAddress);

        /**
         * @brief Handles a subscribe request.
         * Creates a Topic if it does not exist.
         * Returns a Success Status Code, if the user is subscribed.
         * @param ipAddress ip address of the client
         * @param port port of the client to receive Messages
         * @param topicName Topic to subscribe to
         * @return Response 
         */
        Response handleSubscribeRequest(string ipAddress, int port, string topicName);

        /**
         * @brief Handles a unsubscribe request. If the user is not longer subscribed to any topic, the clientConnection will be deleted.
         * Returns a Failed Status Code, if the topic does not exist.
         * Returns a Invalid Parameters Status Code, if the Topic does not exist.
         * Returns a Success Status Code, if the topic exists and the user is unsubscribed.
         * @param ipAddress ip address of the client
         * @param port port of the client to receive Messages
         * @param topicName Topic to unsubscribe from
         * @return Response 
         */
        Response handleUnsubscribeRequest(string ipAddress, int port, string topicName);

        /**
         * @brief Handles a list topics request.
         * Returns a Success Status Code and a list of all topics.
         * @return Response 
         */
        Response handleListTopics();

        /**
         * @brief Handles a get topic status request.
         * Returns a Invalid Parameter Status Code, if the topic does not exist.
         * Returns a Success Status Code and the topic status(MessageTimestamp, Subscriber[ip:port]), if the topic exists.
         * @param topicName Topic to get the status from
         * @return Response 
         */
        Response handleGetTopicStatus(string topicName);

        /**
         * @brief Handles a publish request.
         * Returns a Invalid Parameter Status Code, if the topic does not exist.
         * Returns a Success Status Code, if the topic exists and the message was published.
         * @param topicName Topic to publish to
         * @param message Message to publish
         * @return Response 
         */
        Response handlePublishRequest(string topicName, string message);
};

#endif