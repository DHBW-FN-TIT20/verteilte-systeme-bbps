#include "topic.h"
#include <iostream>
#include <algorithm>
using namespace std;

Topic::Topic(string name)
{
    cout << "Creating topic " << name << endl;
    this->topicName = name;
}

Topic::~Topic()
{
    cout << "Destroying topic" << endl;
}

bool Topic::hasClient(string address, int port)
{
    // Check if there is a client with the given address and port in ClientConnections
    for (ClientConnection *clientConnection : this->subscriberList)
    {
        if (clientConnection->getAddress() == address && clientConnection->getPort() == port)
        {
            return true;
        }
    }
    return false;
}

time_t Topic::getTimeoutTimestamp()
{
    return time_t();
}

void Topic::setTimeoutTimestamp(time_t timestamp)
{
    this->timeoutTimestamp = timestamp;
}

void Topic::publishMessage()
{
    // Send Message to all subscribers
    for (ClientConnection *clientConnection : this->subscriberList)
    {
        clientConnection->SendMessage(this->topicName, this->message, this->messageTimestamp);
    }
}

bool Topic::setMessage(string message)
{
    this->message = message;
    this->messageTimestamp = time(nullptr);
    this->timeoutTimestamp = time(nullptr);
    return true;
}

time_t Topic::getMessageTimestamp()
{
    return time_t();
}

bool Topic::subscribe(ClientConnection *clientConnection)
{
    // Check if clientConnection is already subscribes
    if (this->hasClient(clientConnection->getAddress(), clientConnection->getPort()))
    {
        return false;
    }
    this->subscriberList.push_back(clientConnection);
    return true;
}

bool Topic::unsubscribe(ClientConnection *clientConnection)
{
    // Remove the clientConnection from the subscriberList
    for (ClientConnection *clientConnection : this->subscriberList)
    {
        if (clientConnection->getAddress() == clientConnection->getAddress() && clientConnection->getPort() == clientConnection->getPort())
        {
            this->subscriberList.erase(remove(this->subscriberList.begin(), this->subscriberList.end(), clientConnection), this->subscriberList.end());
            return true;
        }
    }
    return false;
}

string Topic::getTopicName()
{
    return this->topicName;
}

vector<ClientConnection *> Topic::getClientConnections()
{
    // Return the subscriberList
    return this->subscriberList;
}
