/**
 * @file commandIdentifiers.h
 * @brief This file contains a Enum that describes the different Commands that can be sent from the client to the server.
 * @version 0.1
 * @date 2023-06-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COMMANDIDENTIFIERS_H
#define COMMANDIDENTIFIERS_H

/**
 * @brief Specifies a Enum that describes the different Commands that can be sent from the client to the server.
 */
enum CommandIdentifiers{
  subscribe,
  unsubscribe,
  publish,
  listTopics,
  getTopicStatus,
};

#endif // COMMANDIDENTIFIERS_H