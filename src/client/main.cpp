#include <iostream>

#include "client.h"

using namespace std;

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
int main() {
    cout << "Server is running" << endl;
    return 0;
}