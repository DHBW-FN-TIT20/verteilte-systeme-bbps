/**
 * @file commands.cpp
 * @brief Implements a class that represents a command that can be sent from the client to the server.
 * @version 0.1
 * @date 2023-06-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <jsoncpp/json/json.h>
#include <spdlog/spdlog.h>

#include "command.h"
#include "communicationParameters.h"

/**
 * @brief A dictionary that maps command identifiers to their respective argument names.
 * This is used to construct a command object with the correct number of arguments.
 */
static map<CommandIdentifiers, vector<CommunicationParameters>> commandDictionary = {
    {CommandIdentifiers::SUBSCRIBE, {CommunicationParameters::TOPIC_NAME, CommunicationParameters::CLIENT_PORT}},
    {CommandIdentifiers::UNSUBSCRIBE, {CommunicationParameters::TOPIC_NAME}},
    {CommandIdentifiers::PUBLISH, {CommunicationParameters::TOPIC_NAME, CommunicationParameters::MESSAGE}},
    {CommandIdentifiers::LIST_TOPICS, {}},
    {CommandIdentifiers::GET_TOPIC_STATUS, {CommunicationParameters::TOPIC_NAME}}
};

Command::Command(CommandIdentifiers commandIdentifier) {
    // Get the argument names for the command
    vector<CommunicationParameters> commandArgumentNames = commandDictionary[commandIdentifier];

    this->commandIdentifier = commandIdentifier;
    this->availableCommandArguments = commandArgumentNames;

    // Initialize the command argument values array with empty strings for easy access.
    for (int i = 0; i < commandArgumentNames.size(); i++) {
        this->commandArguments.push_back("");
    }
}

Command::~Command() {}

bool Command::setCommandArgument(CommunicationParameters argumentName, string argumentValue) {
    // Check if the argument exists
    if (find(this->availableCommandArguments.begin(), this->availableCommandArguments.end(), argumentName) == this->availableCommandArguments.end()) {
        spdlog::error("Command argument does not exist: {}", argumentName);
        return false;
    }

    // Get the index of the argument
    int argumentIndex = distance(this->availableCommandArguments.begin(), find(this->availableCommandArguments.begin(), this->availableCommandArguments.end(), argumentName));

    // Set the argument value
    this->commandArguments[argumentIndex] = argumentValue;

    return true;
}

string Command::getCommandArgument(CommunicationParameters argumentName) const {
    // Check if the argument exists
    if (find(this->availableCommandArguments.begin(), this->availableCommandArguments.end(), argumentName) == this->availableCommandArguments.end()) {
        spdlog::error("Command argument does not exist: {}", argumentName);
        throw logic_error("Command argument does not exist");
    }

    // Get the index of the argument
    int argumentIndex = distance(this->availableCommandArguments.begin(), find(this->availableCommandArguments.begin(), this->availableCommandArguments.end(), argumentName));

    // Get the argument value
    return this->commandArguments[argumentIndex];
}

bool Command::isValid() const {
    // Check if all arguments are set
    for (int i = 0; i < this->commandArguments.size(); i++) {
        if (this->commandArguments[i] == "") {
            spdlog::debug("Command argument is not set: {}", this->availableCommandArguments[i]);
            return false;
        }
    }

    return true;
}

string Command::serialize() const {
    // Check if all arguments are set
    if (!this->isValid()) {
        spdlog::error("Not all arguments are set");
        throw logic_error("Not all arguments are set");
    }

    Json::Value root;
    root["command"] = static_cast<int>(commandIdentifier);

    Json::Value arguments;
    for (int i = 0; i < this->commandArguments.size(); i++) {
        arguments[communicationParameterToStringDictionary[this->availableCommandArguments[i]]] = this->commandArguments[i];
    }

    root["arguments"] = arguments;

    // stringify JSON object
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";

    return Json::writeString(builder, root);
}

Command Command::deserialize(string serializedCommand) {
    // Parse the JSON string
    Json::Value root;
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    string errors;

    const bool is_parsed = reader->parse(serializedCommand.c_str(), serializedCommand.c_str() + serializedCommand.size(), &root, &errors);

    if (!is_parsed) {
        spdlog::error("Failed to parse JSON command: {}", errors);
        throw logic_error("Failed to parse JSON command");
    }

    if (!root.isMember("command") || !root.isMember("arguments")) {
        spdlog::error("Command does not have all required values");
        throw logic_error("Command does not have all required values");
    }

    // Get the command identifier
    CommandIdentifiers commandIdentifier = static_cast<CommandIdentifiers>(root["command"].asInt());

    // Create a new command object
    Command command(commandIdentifier);

    // Get the arguments
    Json::Value arguments = root["arguments"];
    vector<string> keys = arguments.getMemberNames();

    // Set the arguments
    for (int i = 0; i < keys.size(); i++) {
        if (!command.setCommandArgument(stringToCommunicationParameterDictionary[keys[i]], arguments[keys[i]].asString())) {
            spdlog::error("Failed to set command argument: {}", keys[i]);
            throw logic_error("Failed to set command argument");
        }
    }

    // Check if the command is valid
    if (!command.isValid()) {
        spdlog::error("Command is not valid, there are missing arguments.");
        throw logic_error("Command is not valid, there are missing arguments.");
    }

    return command;
}