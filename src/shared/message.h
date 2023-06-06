/**
 * @file message.h
 * @brief Defines a class that represents a message that can be sent from the server to the client.
 * @version 0.1
 * @date 2023-06-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

using namespace std;

/**
 * @brief Defines a class that represents a message that can be sent from the server to the client.
 * 
 */
class Message {
    public:
        /**
         * @brief Construct a new Message object
         * 
         * @param topic The topic of the message.
         * @param messageTimestamp The timestamp of the message.
         * @param message The message.
         */
        Message(string topic, time_t messageTimestamp, string message);

        /**
         * @brief Destroy the Message object
         * 
         */
        ~Message();

        /**
         * @brief Get the topic of the message.
         * 
         * @return string topic name
         */
        string getTopic() const;

        /**
         * @brief Get the timestamp of the message.
         * 
         * @return time_t Timestamp of the message.
         */
        time_t getMessageTimestamp() const;

        /**
         * @brief Get the message.
         * 
         * @return string The message.
         */
        string getMessage() const;

        /**
         * @brief Serialize the message to a JSON string.
         * 
         * @return string JSON string of the message object.
         */
        string serialize() const;

        /**
         * @brief Deserialize a JSON string to a message object.
         * 
         * @throws runtime_error if the JSON string is invalid.
         * @param serializedMessage JSON string of the message object.
         * @return Message The extracted message object.
         */
        static Message deserialize(string serializedMessage);
    private:
        string topic;
        time_t messageTimestamp;
        string message;
};

#endif // MESSAGE_H