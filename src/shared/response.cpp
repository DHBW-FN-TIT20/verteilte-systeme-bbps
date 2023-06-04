/**
 * @file response.cpp
 * @brief
 * @version 0.1
 * @date 2023-06-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <jsoncpp/json/json.h>
#include <spdlog/spdlog.h>

#include "response.h"
#include "communicationParameters.h"

static map<CommandIdentifiers, vector<CommunicationParameters>> responseDictionary = {
    {CommandIdentifiers::SUBSCRIBE, {}},
    {CommandIdentifiers::UNSUBSCRIBE, {}},
    {CommandIdentifiers::PUBLISH, {}},
    {CommandIdentifiers::LIST_TOPICS, {CommunicationParameters::TOPICS}},
    {CommandIdentifiers::GET_TOPIC_STATUS, {CommunicationParameters::MESSAGE_TIMESTAMP, CommunicationParameters::SUBSCRIBERS}}};

Response::Response(CommandIdentifiers commandIdentifier) {
    // Get the argument names for the response
    vector<CommunicationParameters> responseArgumentNames = responseDictionary[commandIdentifier];

    this->availableResponseArguments = responseArgumentNames;

    // Initialize the response argument values array with empty strings for easy access.
    for (int i = 0; i < responseArgumentNames.size(); i++)
    {
        this->responseArguments.push_back("");
    }
}

Response::~Response() {}

bool Response::setResponseArgument(CommunicationParameters argumentName, string argumentValue)
{
    // Check if the argument exists
    if (find(this->availableResponseArguments.begin(), this->availableResponseArguments.end(), argumentName) == this->availableResponseArguments.end())
    {
        spdlog::error("Response argument does not exist: {}", communicationParameterToStringDictionary[argumentName]);
        return false;
    }

    // Get the index of the argument
    int argumentIndex = distance(this->availableResponseArguments.begin(), find(this->availableResponseArguments.begin(), this->availableResponseArguments.end(), argumentName));

    // Set the argument value
    this->responseArguments[argumentIndex] = argumentValue;

    return true;
}

bool Response::setResponseArgument(CommunicationParameters argumentName, vector<string> argumentValue)
{
    // Convert the argumentValue vector to a string
    string argumentValueString = "[";
    for (int i = 0; i < argumentValue.size(); i++)
    {
        argumentValueString += argumentValue[i];
        if (i != argumentValue.size() - 1)
        {
            argumentValueString += ",";
        }
    }
    argumentValueString += "]";

    return setResponseArgument(argumentName, argumentValueString);
}

bool Response::setResponseArgument(CommunicationParameters argumentName, uint64_t argumentValue)
{
    return setResponseArgument(argumentName, to_string(argumentValue));
}

string Response::getResponseArgument(CommunicationParameters argumentName) const
{
    // Check if the argument exists
    if (find(this->availableResponseArguments.begin(), this->availableResponseArguments.end(), argumentName) == this->availableResponseArguments.end())
    {
        spdlog::error("Response argument does not exist: {}", communicationParameterToStringDictionary[argumentName]);
        throw runtime_error("Response argument does not exist");
    }

    // Get the index of the argument
    int argumentIndex = distance(this->availableResponseArguments.begin(), find(this->availableResponseArguments.begin(), this->availableResponseArguments.end(), argumentName));

    // Get the argument value
    return this->responseArguments[argumentIndex];
}

vector<CommunicationParameters> Response::getAvailableArguments() const
{
    return this->availableResponseArguments;
}

bool Response::setStatusCode(Statuscode statusCode)
{
    this->statusCode = statusCode;
    return true;
}

Statuscode Response::getStatusCode() const
{
    return this->statusCode;
}

string Response::serialize() const
{
    Json::Value root;
    // statusCode as int
    root["statusCode"] = static_cast<int>(this->statusCode);

    for (int i = 0; i < this->availableResponseArguments.size(); i++)
    {
        root[communicationParameterToStringDictionary[this->availableResponseArguments[i]]] = this->responseArguments[i];
    }

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";

    return Json::writeString(builder, root);
}

Response Response::deserialize(CommandIdentifiers commandIdentifier, string serializedResponse)
{
    Json::Value root;
    Json::CharReaderBuilder builder;
    Json::CharReader *reader = builder.newCharReader();
    string errors;

    bool parsingSuccessful = reader->parse(serializedResponse.c_str(), serializedResponse.c_str() + serializedResponse.size(), &root, &errors);
    delete reader;

    if (!parsingSuccessful)
    {
        spdlog::error("Failed to parse JSON: {}", errors);
        throw runtime_error("Failed to parse JSON");
    }

    Response response(commandIdentifier);

    try {
        // Set the status code
        response.setStatusCode(static_cast<Statuscode>(root["statusCode"].asInt()));

        // Set the response arguments
        for (int i = 0; i < response.availableResponseArguments.size(); i++)
        {
            response.setResponseArgument(response.availableResponseArguments[i], root[communicationParameterToStringDictionary[response.availableResponseArguments[i]]].asString());
        }
    } catch (const invalid_argument &e) {
        spdlog::error("Invalid or missing arguments: {}", e.what());
        throw runtime_error("Invalid or missing arguments.");
    }

    return response;
}