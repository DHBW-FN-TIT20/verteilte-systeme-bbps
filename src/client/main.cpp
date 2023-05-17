#include <iostream>
#include <sstream>
#include <spdlog/spdlog.h>
#include <list>

#include "client.h"

using namespace std;

bool isValidIPv4Address(const string &ipAddress);
int getPortNumber(const string &portStr);

/**
 * @brief Entry point of the client application.
 * 
 * @return int 
 * 
 * @note Example usage:
 * @code {.sh}
 *  subscriber_client --publisher-server <address> --register-topic TOPIC_NAME_1
 *  subscriber_client --publisher-server <address> --publish TOPIC_NAME_1 TOPIC_DATA_1
 * @endcode
 */
int main(int argc, char** argv) {
    spdlog::info("Initializing client...");
    spdlog::info("Parsing CLI arguments...");

    int port = 0;
    string serverAddress = "";
    int serverPort = -1;
    list<list<string>> commands = {};

    // Parse CLI arguments and validate their values
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "--port" && argc > i + 1) {
            string portStr = argv[i + 1];

            port = getPortNumber(portStr);

            if (port == -1) {
                spdlog::error("Invalid server port: {}", portStr);
                spdlog::error("Shutting down...");
                return 1;
            }

            spdlog::info("Port: {}", port);

            i += 1;
        } else if (arg == "--server-address" && argc > i + 1) {
            serverAddress = argv[i + 1];

            if (!isValidIPv4Address(serverAddress)) {
                spdlog::error("Invalid server address: {}", serverAddress);
                spdlog::error("Shutting down...");
                return 1;
            }

            spdlog::info("Server address: {}", serverAddress);

            i += 1;
        } else if (arg == "--server-port" && argc > i + 1) {
            string serverPortStr = argv[i + 1];

            serverPort = getPortNumber(serverPortStr);

            if (serverPort == -1) {
                spdlog::error("Invalid server port: {}", serverPortStr);
                spdlog::error("Shutting down...");
                return 1;
            }

            spdlog::info("Server-Port: {}", port);

            i += 1;
        } else if (arg == "--subscribe" && argc > i + 1) {
            string topicName = argv[i + 1];

            list<string> command = {"subscribe", topicName};
            commands.push_back(command);

            spdlog::info("-> Subscribing to topic: {}", topicName);

            i += 1;
        } else if (arg == "--unsubscribe" && argc > i + 1) {
            string topicName = argv[i + 1];

            list<string> command = {"unsubscribe", topicName};
            commands.push_back(command);

            spdlog::info("-> Unsubscribing from topic: {}", topicName);

            i += 1;
        } else if (arg == "--publish" && argc > i + 2) {
            string topicName = argv[i + 1];
            string topicData = argv[i + 2];

            list<string> command = {"publish", topicName, topicData};
            commands.push_back(command);

            spdlog::info("-> Publishing data: {} to topic: {}", topicData, topicName);

            i += 2;
        } else if (arg == "--list-topics") {
            list<string> command = {"list-topics"};
            commands.push_back(command);

            spdlog::info("-> List topics");
        } else if (arg == "--get-topic-status" && argc > i + 1) {
            string topicName = argv[i + 1];

            list<string> command = {"get-topic-status", topicName};
            commands.push_back(command);

            spdlog::info("-> Get topic status for topic: {}", topicName);

            i += 1;
        } else {
            // Unknown CLI argument or missing values
            spdlog::error("Unknown argument or missing values for: {}", arg);
            spdlog::error("Shutting down...");
            return 1;
        }
    }

    spdlog::info("Successfully parsed CLI arguments.");
    spdlog::info("Starting client...");

    Client* client;
    try {
        client = new Client(port, serverAddress, serverPort);
    } catch (const exception &e) {
    }

    return 0;
}

int getPortNumber(const string& portStr) {
    int port = -1;

    try {
        port = stoi(portStr, nullptr, 10);
    } catch (const exception &e) {
        return -1;
    }

    // Port numbers are available from 0 to 65535
    if (port > 65535) {
        return -1;
    }

    return port;
}

bool isValidIPv4Address(const string& ipAddress) {
    int octetCount = 0;
    stringstream ss(ipAddress);
    string octet;

    if (ipAddress.back() == '.')
        return false;

    while (getline(ss, octet, '.')) {
        try {
            int value = std::stoi(octet);

            if (value < 0 || value > 255)
                return false;

            octetCount++;
        } catch (const invalid_argument&) {
            return false;
        } catch (const std::out_of_range&) {
            return false;
        }
    }

    if (octetCount != 4)
        return false;

    return true;
}