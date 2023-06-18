#ifndef TOPIC_H
#define TOPIC_H

#include <string>
#include <ctime>
#include <vector>

#include "clientconnection.h"

using namespace std;

class Topic {
    public:
        /**
         * @brief Construct a new Topic object
         * 
         * @param name Topic name
         */
        Topic(string name);

        /**
         * @brief Destroy the Topic object
         * 
         */
        ~Topic();

        /**
         * @brief Evaluates whether the given subscriber is subscribed to this topic.
         * 
         * @param address IPv4 address of the subscriber.
         * @param port Port of the subscriber.
         * @return true if the subscriber is subscribed to this topic.
         * @return false if the subscriber is not subscribed to this topic.
         */
        bool hasClient(string address, int port);

        /**
         * @brief Get the \p timeoutTimestamp value.
         * 
         * @return time_t Current timeout timestamp.
         */
        time_t getTimeoutTimestamp();

        /**
         * @brief Set the \p timeoutTimestamp value.
         * 
         * @param timestamp New timeout timestamp.
         * 
         * @details
         * The timeout timestamp must be in the future.
         */
        void setTimeoutTimestamp(time_t timestamp);

        /**
         * @brief Send the \p message and \p messageTimestamp to all subscribers.
         * 
         */
        void publishMessage();

        /**
         * @brief Set the \p message value and update the \p messageTimestamp.
         * 
         * @param message New message.
         */
        bool setMessage(string message);

        /**
         * @brief Get the \p messageTimestamp value
         * 
         * @return time_t 
         */
        time_t getMessageTimestamp();

        /**
         * @brief Add a subscriber to the \p subscriberList.
         * 
         * @param clientConnection Pointer to the subscriber.
         * @return true if the subscriber was added.
         * @return false if the subscriber was not added.
         */
        bool subscribe(ClientConnection* clientConnection);

        /**
         * @brief Remove a subscriber from the \p subscriberList.
         * 
         * @param clientConnection Pointer to the subscriber.
         * @return true if the subscriber was removed.
         * @return false if the subscriber was not removed.
         */
        bool unsubscribe(ClientConnection* clientConnection);

        /**
         * @brief Get the Topic Name object
         * 
         * @return string the Topic Name
         */
        string getTopicName();

        /**
         * @brief Get the Client Connections List
         * 
         * @return vector<ClientConnection*> List of all ClientConnections
         */
        vector<ClientConnection*> getClientConnections();

    private:
        /**
         * @brief Stores the latest Message
         * 
         */
        string message;

        /**
         * @brief Stores the name of the topic
         * 
         */
        string topicName;
        time_t messageTimestamp;
        vector<ClientConnection*> subscriberList;
        time_t timeoutTimestamp;
};

#endif