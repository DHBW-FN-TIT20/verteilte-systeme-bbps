#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <string>
#include <ctime>

using namespace std;

class ClientConnection
{
    public:
        /**
         * @brief Construct a new Client Connection object
         * Creates the Socket with the client.
         * @param address IPv4 Address of the client
         * @param port Port of the client
         */
        ClientConnection(string address, int port);
        
        /**
         * @brief Destroy the Client Connection object
         * Closes the socket.
         */
        ~ClientConnection();

        /**
         * @brief Sends a message to the client using the websocket
         * The sent status code is always 0 (successful).
         * @param message Message to send
         * @param timestamp Timestamp of the message
         */
        void SendMessage(string message, time_t timestamp);
    
    private:
        string address;
        int port;
        int socket;
};

#endif