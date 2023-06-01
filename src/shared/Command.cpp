/**
 * @file Commands.cpp
 * @author your name (you@domain.com)
 * @brief Specifies a static list of commands that can be used by the client and send to the server.
 * @version 0.1
 * @date 2023-05-31
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Command.h"

/**
 * @brief Specifies a static list of commands that can be used by the client and send to the server.
 */
Command commands[] = {
    {CommandIdentifiers::SUBSCRIBE, {"topicName"}},
    {CommandIdentifiers::UNSUBSCRIBE, {"topicName"}},
    {CommandIdentifiers::PUBLISH, {"topicName", "message"}},
    {CommandIdentifiers::LIST_TOPICS, {}},
    {CommandIdentifiers::GET_TOPIC_STATUS, {"topicName"}},
};

string Command::serialize() const
{
    throw logic_error("Function not implemented yet"); 
}

Command Command::deserialize(string)
{
    throw logic_error("Function not implemented yet"); 
}