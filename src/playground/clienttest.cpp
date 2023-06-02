/**
 * @file clienttest.cpp
 * @author Lukas Braun
 * @brief File to test the connection with the server
 * @version 0.1
 * @date 2023-05-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    std::string serverIP = "127.0.0.1"; // Replace with the server's IP address
    int port = 8080; // Replace with the server's port number
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    if (inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "Invalid address or address not supported" << std::endl;
        return -1;
    }

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Error connecting to server" << std::endl;
        return -1;
    }

    std::string message = "Hello server!";
    std::string MultilineString =
        "{"
            "\"command\": 0,"
            "\"arguments\": {"
            "\"topicName\": \"TEST\""
            "}"
        "}";
    if (send(clientSocket, MultilineString.c_str(), MultilineString.length(), 0) < 0) {
        std::cerr << "Error sending data to server" << std::endl;
        return -1;
    }

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead < 0) {
        std::cerr << "Error receiving response from server" << std::endl;
        return -1;
    }

    std::string response(buffer, bytesRead);
    std::cout << "Received from server: " << response << std::endl;

    close(clientSocket);

    return 0;
}
