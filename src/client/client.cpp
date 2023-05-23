#include "client.h"

#include "../include/spdlog/spdlog.h"

using namespace std;

Client::Client(int port, string serverAddress, int serverPort) {
    spdlog::info("Initializing client with port {}, server address {} and server port {}...", port, serverAddress, serverPort);
}

Client::~Client() {
    spdlog::info("Terminating client...");
}

void Client::subscribeTopic(string topicName) {
    spdlog::info("Subscribing to topic: {}", topicName);
}

void Client::unsubscribe(string topicName) {
    spdlog::info("Unsubscribing from topic: {}", topicName);
}

void Client::publishTopic(string topicName, string message) {
    spdlog::info("Publishing message: {} to topic: {}", message, topicName);
}

void Client::listTopics() {
    spdlog::info("Listing topics...");
}

void Client::getTopicStatus(string topicName) {
    spdlog::info("Getting status of topic: {}", topicName);
}