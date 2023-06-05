/**
 * @file response.h
 * @brief Defines a class that represents a server response to a client command.
 * @version 0.1
 * @date 2023-06-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <vector>

#include "commandIdentifiers.h"
#include "communicationParameters.h"
#include "statuscode.h"

using namespace std;

/**
 * @brief Defines a class that represents a server response to a client command.
 * 
 */
class Response {
    public:
        /**
         * @brief Construct a new Response object
         * 
         * @param commandIdentifier 
         */
        Response(CommandIdentifiers commandIdentifier);

        /**
         * @brief Destroy the Response object
         * 
         */
        ~Response();

        /**
         * @brief Set the Response Argument value if the argument exists.
         * 
         * @param argumentName Name of the argument.
         * @param argumentValue Value of the argument.
         * @return true if the argument was set successfully.
         * @return false if the argument does not exist or cannot be set.
         */
        bool setResponseArgument(CommunicationParameters argumentName, string argumentValue);

        /**
         * @brief Set the Response Argument value to a vector of strings.
         * 
         * @param argumentName Name of the argument.
         * @param argumentValue Value (list of strings) of the argument.
         * @return true if the argument was set successfully.
         * @return false if the argument does not exist or cannot be set.
         */
        bool setResponseArgument(CommunicationParameters argumentName, vector<string> argumentValue);

        /**
         * @brief Set the Response Argument value to a uint64_t.
         * 
         * @param argumentName Name of the argument.
         * @param argumentValue Value (uint64_t) of the argument.
         * @return true if the argument was set successfully.
         * @return false if the argument does not exist or cannot be set.
         */
        bool setResponseArgument(CommunicationParameters argumentName, uint64_t argumentValue);

        /**
         * @brief Get the Response Argument value as string.
         * 
         * @throws logic_error if the argument does not exist.
         * @param argumentName Name of the argument.
         * @return string Value of the argument.
         */
        string getResponseArgument(CommunicationParameters argumentName) const;

        /**
         * @brief Get a list of the available argument names.
         * 
         * @return vector<string> List of available argument names.
         */
        vector<CommunicationParameters> getAvailableArguments() const;

        /**
         * @brief Set the response status code
         * 
         * @param statusCode
         * @return true if the status code was set successfully.
         * @return false if the status code cannot be set.
         */
        bool setStatusCode(Statuscode statusCode);

        /**
         * @brief Get the Status Code of the response.
         * 
         * @return Statuscode 
         */
        Statuscode getStatusCode() const;

        /**
         * @brief Serialize the response object to a JSON string.
         * 
         * @return string JSON string.
         */
        string serialize() const;

        /**
         * @brief Deserialize a JSON string to a response object.
         * 
         * @throws logic_error if the JSON string is invalid (cannot be parsed).
         * @param commandIdentifier Corresponding command identifier.
         * @param serializedResponse JSON serialized response.
         * @return Response 
         */
        static Response deserialize(CommandIdentifiers commandIdentifier, string serializedResponse);
    private:
        Statuscode statusCode;
        vector<CommunicationParameters> availableResponseArguments;
        vector<string> responseArguments;
};

#endif // RESPONSE_H