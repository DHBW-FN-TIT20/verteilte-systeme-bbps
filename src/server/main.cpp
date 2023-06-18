#include <spdlog/spdlog.h>
#include <iostream>

#include "server.h"

using namespace std;

void printUsageInformation() {
    cout << endl << "Commandline Tool to start the publisher server" << endl;
    cout << endl << "Options:" << endl;
    cout << "    " <<"--help: " << endl;
    cout << "    " <<"    " <<"Print this help message" << endl;
    cout << "    " <<"--port: " << endl;
    cout << "    " <<"    " <<"Port to listen on" << endl;
    cout << "    " <<"    " <<"Default: 8080" << endl;
    cout << "    " <<"--topic-timeout: " << endl;
    cout << "    " <<"    " <<"Timeout in seconds for all topics" << endl;
    cout << "    " <<"    " <<"Default: 30" << endl;
    cout << endl << "Debug:" << endl;
    cout << "    " <<"--debug: " << endl;
    cout << "    " <<"    " <<"Enable logging debug information" << endl;
    cout << endl;
}

/**
 * @brief Entry point of the server application.
 * 
 * @return int 
 * 
 * @note Example usage:
 * @code {.sh}
 *  publisher_server --port <port> --message-timeout <timeout in s>
 * @endcode
 */
int main(int argc, char* argv[]) {
    // The first argument (argv[0]) is the name of the program itself
    int port = 8080;
    int topicTimeout = 30;
    // Check if there are furtheral arguments
    if (argc > 1) {
        // Access and interpret the arguments
        for (int i = 1; i < argc; i++) {
            string arg = argv[i];
            if (arg == "--help") {
                // Print usage information
                printUsageInformation();
                // End the program
                return 0;
            }
            else if (arg == "--port") {
                // Try to convert it to an integer
                try
                {
                    // Convert the string to an integer
                    // and store it in the messageTimeout variable
                    port = stoi(argv[i+1]);
                }
                catch(const exception& e)
                {
                    spdlog::error("--port musst be a number.");
                    // End the program with an error
                    return 1;
                }
                //check if port is in range
                if (port < 0 || port > 65535) {
                    spdlog::error("--port musst be in range 0-65535.");
                    // End the program with an error
                    return 1;
                }
                i++;
            }
            else if (arg == "--topic-timeout") {
                // Try to convert it to an integer
                try
                {
                    // Convert the string to an integer
                    // and store it in the messageTimeout variable
                    topicTimeout = stoi(argv[i+1]);
                }
                catch(const exception& e)
                {
                    spdlog::error("--topic-timeout musst be a number.");
                    // End the program with an error
                    return 1;
                }
                i++;
            } else if (arg == "--debug") {
                spdlog::set_level(spdlog::level::debug);
            } else {
                // Invalid argument
                spdlog::error("Invalid arguments: ");
                // Print usage information
                printUsageInformation();
                // End the program with an error
                return 1;
            }
        }
    } 
    else {
        // No arguments provided
        spdlog::error("No arguments provided. Use --help to see the available Options");
        // Print usage information
    }
    // Create a new server object
    Server server(port, topicTimeout);
}
