#ifndef STATUSCODE_H
#define STATUSCODE_H

/**
 * @brief Specifies a Enum that describes the different status codes that can be sent from the server to the client.
 * 
 */
enum Statuscode {
    success,
    failed,
    invalidParameter,
    internalError
};

#endif // STATUSCODE_H