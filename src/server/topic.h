#ifndef TOPIC_H
#define TOPIC_H

#include <string>
#include <ctime>
#include <list>

#include "clientconnection.h"

using namespace std;

class Topic
{
    private:
        string messages;
        time_t messageTimestamp; 
        list<ClientConnection*> subscribersList;
        time_t timeoutTimestamp;

    public:
        Topic(string name, int timeoutInSeconds);
        bool hasClient(string address, int port);
        time_t getTimeoutTimestamp();
        void setTimeoutTimestamp(time_t timestamp);
        bool publishMessage();
        void setMessage(string message);
        time_t getMessageTimestamp();
};

#endif