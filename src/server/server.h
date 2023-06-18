#ifndef SERVER_H
#define SERVER_H

#include <ctime>
#include <thread>
#include <vector>

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
         */#include <vector>
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
        vector<Topic*> topics;
        vector<ClientConnection*> clientConnections;
        thread timeoutCheckerThread;
        void startServer(int port, int topicTimeout);

        /**
         * @brief Handles a approaching client.
         * 
         * @param clientSocket client socket
         * @param clientAddress client address structure
         */
        void handleApproachingClient(int clientSocket, struct sockaddr_in* clientAddress);

        /**
         * @brief Handles a subscribe request.
         * Returns a Failed Status Code, if the topic does not exist.
         * Returns a Invalid Parameters Status Code, if the Topic does not exist.
         * Returns a Success Status Code, if the topic exists and the user is subscribed.
         * @param ipAddress ip address of the client
         * @param port port of the client to recieve Mesages
         * @param topicName Topic to subscribe to
         * @return Response 
         */
        Response handleSubsscribeRequest(string ipAddress, int port, string topicName);

        /**
         * @brief Handles a unsubscribe request. If the user is not longer subscribed to any topic, the clientConnection will be deleted.
         * Returns a Failed Status Code, if the topic does not exist.
         * Returns a Invalid Parameters Status Code, if the Topic does not exist.
         * Returns a Success Status Code, if the topic exists and the user is unsubscribed.
         * @param ipAddres ip address of the client
         * @param port port of the client to recieve Mesages
         * @param topicName Topic to unsubscribe from
         * @return Response 
         */
        Response handleUnsubscribeRequest(string ipAddres, int port, string topicName);

        /**
         * @brief Handles a list topics request.
         * Returns a Success Status Code and a list of all topics.
         * @return Response 
         */
        Response handleListTopics();

        /**
         * @brief Handles a get topic status request.
         * Returns a Failed Status Code, if the topic does not exist.
         * Returns a Success Status Code and the topic status(MessageTimestamp, Subscriber), if the topic exists.
         * @param topicName Topic to get the status from
         * @return Response 
         */
        Response handleGetTopicStatus(string topicName);

        /**
         * @brief Handles a publish request.
         * Returns a Failed Status Code, if the topic does not exist.
         * Returns a Succes Status Code, if the topic exists and the message was published.
         * @param topicName Topic to publish to
         * @param message Message to publish
         * @return Response 
         */
        Response handlePublishRequest(string topicName, string message);
};

#endif