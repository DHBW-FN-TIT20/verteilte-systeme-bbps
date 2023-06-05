/**
 * @file communicationParameters.h
 * @brief This file contains a Enum that describes the different parameter names for the JSON communication, as well as functions to convert Enum <-> string.
 * @version 0.1
 * @date 2023-06-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMMUNICATION_PARAMETERS_H
#define COMMUNICATION_PARAMETERS_H

#include <string>

using namespace std;

/**
 * @brief Specifies a Enum that describes the different parameter names for the JSON communication.
 * This Enum reduces the risk of typos when accessing the JSON communication parameters.
 */
enum CommunicationParameters {
    TOPIC_NAME,
    CLIENT_PORT,
    MESSAGE,
    TOPICS,
    MESSAGE_TIMESTAMP,
    SUBSCRIBERS,
};

/**
 * @brief Maps a CommunicationParameters Enum to a string.
 * 
 */
static map<CommunicationParameters, string> communicationParameterToStringDictionary = {
    {CommunicationParameters::TOPIC_NAME, "topicName"},
    {CommunicationParameters::CLIENT_PORT, "clientPort"},
    {CommunicationParameters::MESSAGE, "message"},
    {CommunicationParameters::TOPICS, "topics"},
    {CommunicationParameters::MESSAGE_TIMESTAMP, "messageTimestamp"},
    {CommunicationParameters::SUBSCRIBERS, "subscribers"}
};

/**
 * @brief Maps a string to a CommunicationParameters Enum.
 * 
 */
static map<string, CommunicationParameters> stringToCommunicationParameterDictionary = {
    {"topicName", CommunicationParameters::TOPIC_NAME},
    {"clientPort", CommunicationParameters::CLIENT_PORT},
    {"message", CommunicationParameters::MESSAGE},
    {"topics", CommunicationParameters::TOPICS},
    {"messageTimestamp", CommunicationParameters::MESSAGE_TIMESTAMP},
    {"subscribers", CommunicationParameters::SUBSCRIBERS}
};

#endif // COMMUNICATION_PARAMETERS_H