#ifndef COMMUNICATION_PARAMETERS_H
#define COMMUNICATION_PARAMETERS_H

#include <string>
#include <map>

using namespace std;

/**
 * @brief Specifies a Enum that describes the different parameter names for the JSON communication.
 * This Enum reduces the risk of typos when accessing the JSON communication parameters.
 */
enum CommunicationParameters {
    topicName,
    clientPort,
    message,
    topics,
    messageTimestamp,
    subscribers,
};

/**
 * @brief Maps a CommunicationParameters Enum to a string.
 * 
 */
static map<CommunicationParameters, string> communicationParameterToStringDictionary = {
    {CommunicationParameters::topicName, "topicName"},
    {CommunicationParameters::clientPort, "clientPort"},
    {CommunicationParameters::message, "message"},
    {CommunicationParameters::topics, "topics"},
    {CommunicationParameters::messageTimestamp, "messageTimestamp"},
    {CommunicationParameters::subscribers, "subscribers"}
};

/**
 * @brief Maps a string to a CommunicationParameters Enum.
 * 
 */
static map<string, CommunicationParameters> stringToCommunicationParameterDictionary = {
    {"topicName", CommunicationParameters::topicName},
    {"clientPort", CommunicationParameters::clientPort},
    {"message", CommunicationParameters::message},
    {"topics", CommunicationParameters::topics},
    {"messageTimestamp", CommunicationParameters::messageTimestamp},
    {"subscribers", CommunicationParameters::subscribers}
};

#endif // COMMUNICATION_PARAMETERS_H