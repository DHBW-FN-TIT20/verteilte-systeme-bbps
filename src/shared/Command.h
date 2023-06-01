/**
 * @file Command.h
 * @author Lukas Braun
 * @brief Defines a struct that represents a command that can be sent from the client to the server.
 * @version 0.1
 * @date 2023-05-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>

#include "CommandIdentifiers.h"

using namespace std;

/**
 * @brief Defines a struct that represents a command that can be sent from the client to the server.
 * 
 */
struct Command {
    /**
     * @brief Specifies the command identifier.
     */
    CommandIdentifiers commandIdentifier;

    /**
     * @brief Specifies the command arguments.
     */
    vector<string> commandArguments;
    
    /**
     * @brief Serializes the command to a json string.
     * @return string The json-serialized command.
     */
    string serialize() const;

    /**
     * @brief Deserializes a json string to a command.
     * @param string The json-serialized command.
     * @return Command The deserialized command.
    */
    static Command deserialize(string);  // Declaration of deserialization function
};

#endif // SHAREDSTRUCTS_H
