/**
 * @file command.h
 * @brief Defines a class that represents a command that can be sent from the client to the server.
 * @version 0.2
 * @date 2023-06-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>

#include "commandIdentifiers.h"
#include "communicationParameters.h"

using namespace std;

/**
 * @brief Defines a class that represents a command that can be sent from the client to the server.
 * 
 */
class Command {
    public:
        /**
         * @brief Construct a new Command object using the specified command identifier.
         * 
         * @param commandIdentifier The command identifier.
         */
        Command(CommandIdentifiers commandIdentifier);

        /**
         * @brief Construct a new Command object
         * 
         */
        Command();

        /**
         * @brief Destroy the Command object
         */
        ~Command();

        /**
         * @brief Set the arguments for the command.
         * 
         * @param argumentName The name of the argument.
         * @param argumentValue The new value of the argument.
         * @return true if the argument was set successfully.
         * @return false if the argument does not exist or cannot be set.
         */
        bool setCommandArgument(CommunicationParameters argumentName, string argumentValue);

        /**
         * @brief Get the Command Argument value.
         * @throws logic_error if the argument does not exist.
         * @param argumentName The name of the argument.
         * @return string The value of the argument.
         */
        string getCommandArgument(CommunicationParameters argumentName) const;

        /**
         * @brief Checks whether all required arguments are set.
         * @return true if the command is valid.
         * @return false if the command is invalid.
         */
        bool isValid() const;

        /**
         * @brief Serializes the command to a json string.
         * @throws logic_error if the command cannot be serialized.
         * @return string The json-serialized command.
         */
        string serialize() const;

        /**
         * @brief Deserializes a json string to a command.
         * @throws logic_error if the command cannot be deserialized or does not have all required values.
         * @param serializedCommand The json-serialized command.
         * @return Command The deserialized command.
         */
        static Command deserialize(string serializedCommand);
        
        /**
         * @brief Specifies the command identifier.
         */
        CommandIdentifiers commandIdentifier;

    private:
        /**
         * @brief Specifies the available command argument names.
         */
        vector<CommunicationParameters> availableCommandArguments;

        /**
         * @brief Specifies the command arguments.
         */
        vector<string> commandArguments;
};

#endif // COMMAND_H
