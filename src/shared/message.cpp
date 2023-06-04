/**
 * @file message.cpp
 * @brief Implements a class that represents a message that can be sent from the server to the client.
 * @version 0.1
 * @date 2023-06-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <string>
#include <jsoncpp/json/json.h>
#include <spdlog/spdlog.h>

#include "message.h"

Message::Message(string topic, time_t messageTimestamp, string message) {
    this->topic = topic;
    this->messageTimestamp = messageTimestamp;
    this->message = message;
}

Message::~Message() {}

string Message::getTopic() const {
    return this->topic;
}

time_t Message::getMessageTimestamp() const {
    return this->messageTimestamp;
}

string Message::getMessage() const {
    return this->message;
}

string Message::serialize() const {
    Json::Value messageJson;
    messageJson["topic"] = this->topic;
    messageJson["messageTimestamp"] = static_cast<uint64_t>(this->messageTimestamp);
    messageJson["message"] = this->message;

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, messageJson);
}

Message Message::deserialize(string serializedMessage) {
    Json::Value messageJson;
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    string errors;

    bool parsingSuccessful = reader->parse(serializedMessage.c_str(), serializedMessage.c_str() + serializedMessage.size(), &messageJson, &errors);
    delete reader;

    if (!parsingSuccessful) {
        spdlog::error("Failed to parse message JSON: {}", errors);
        throw runtime_error("Failed to parse message JSON");
    }

    string topic, message;
    time_t messageTimestamp;

    try {
        topic = messageJson["topic"].asString();
        messageTimestamp = static_cast<time_t>(stoll(messageJson["messageTimestamp"].asString()));
        message = messageJson["message"].asString();
    } catch (const invalid_argument &e) {
        spdlog::error("Invalid or missing arguments in message JSON: {}", e.what());
        throw runtime_error("Invalid or missing arguments in message JSON");
    }

    return Message(topic, messageTimestamp, message);
}