# Publisher / Subscriber System
Abschlusstestat für die Vorlesung Verteilte Systeme and der DHBW Ravensburg im Studiengang Informatik - Informationstechnik des Jahrgangs 2020. <br>
von Johannes Brandenburger, Lukas Braun, Phillip Patzelt, Henry Schuler und Lea Silberzahn

# Table of Contents
- [Publisher / Subscriber System](#publisher--subscriber-system)
- [Table of Contents](#table-of-contents)
- [Scope of Tasks](#scope-of-tasks)
- [Installation](#installation)
- [Instructions](#instructions)
  - [Client](#client)
  - [Server](#server)
- [Technologies](#technologies)
  - [Client and Server (C++)](#client-and-server-c)
    - [JsonCpp](#jsoncpp)
    - [spdlog](#spdlog)
    - [doxygen](#doxygen)
  - [User Tests (Python)](#user-tests-python)
- [Description of System Architecture TODO:](#description-of-system-architecture-todo)
- [Interface Description](#interface-description)
  - [Client request](#client-request)
  - [Server response](#server-response)
    - [Default](#default)
    - [LIST\_TOPIC](#list_topic)
    - [GET\_TOPIC\_STATUS](#get_topic_status)
  - [Server publish (message/heartbeat)](#server-publish-messageheartbeat)
  - [TODO:](#todo)
- [Concurrency](#concurrency)
- [Source Code Documentation](#source-code-documentation)
  - [Error handling](#error-handling)
  - [Doxygen](#doxygen)
- [Testing](#testing)



# Scope of Tasks
> The specific requirements are described in the provided PDF file. <br>
> Within this section, a brief overview of the requirements is given.
- Create a publisher (server) / subscriber (client) system
  - Publisher
    - Manages topics and their subscribers
    - Manages at least 32 topics
    - Informs all subscribers about new messages
      - Periodically sends a heartbeat (resend message) to all subscribers
  - Subscriber
    - Can: subscribe, unsubscribe, publish, list topics and get topic status
# Installation
This software has to be compiled under Linux (tested with ubuntu-22.04.1-desktop-amd64).

1. Install git, make, g++, libjsoncpp-dev
   ```shell
   sudo apt-get install git make g++ libjsoncpp-dev
   ```
2. Clone the repository and compile client and server
   ```shell
   git clone https://github.com/DHBW-FN-TIT20/verteilte-systeme-bbpss.git
   cd verteilte-systeme-bbpss/
   make all
   ```
3. Execute the `client.exe` or `server.exe`
# Instructions
## Client
To start the client, there are several arguments that can be passed.
- Since the SETTING arguments have a default value, they are optional.
- In order to be able to start the server you have to provide exactly **one** COMMAND.
> Info: If the --help argument is used, the client will not start.

type | argument | description | default
--- | --- | --- | ---
INFO | --help | display all available arguments
SETTING | --port &lt;PORT&gt;| port for connections to this client | 0 (random available port)
SETTING | --server-address &lt;ADDRESS&gt; | IPv4 address of the publisher server | 127.0.0.1
SETTING | --server-port &lt;PORT&gt; | listening port of the publisher server | 8080
COMMAND | --subscribe &lt;TOPIC1&gt; &lt;...&gt; &lt;TOPICn&gt; | subscribe to (multiple) topics
COMMAND | --publish &lt;TOPIC&gt; &lt;MESSAGE&gt; | publish a message to a topic
COMMAND | --list-topics | list all existing topics
COMMAND | --get-topic-status &lt;TOPIC1&gt; &lt;...&gt; &lt;TOPICn&gt; | get the status of (multiple) topics
DEBUG | --debug | enable logging debug information

> **Restriction**: When using the `--publish` command, the topic name and message combined cannot exceed 1700 characters. However, all types of characters are allowed.

## Server
A server is started with the following arguments:
> Info: If the --help argument is used, the server will not start.
> 
type | argument | description | default
--- | --- | --- | ---
INFO | --help | display all available arguments
SETTING | --port &lt;PORT&gt; | listening port for the client | 8080
SETTING | --topic-timeout | Inactivity timeout in seconds for all topics. If no message is received in this period, the message is resent to each subscriber (heartbeat). | 30
DEBUG | --debug | enable logging debug information

# Technologies
## Client and Server (C++)
### JsonCpp
[JsonCpp](https://github.com/open-source-parsers/jsoncpp) is used to work with JSON structures in C++.
This library is used to parse the request and response bodies for the communication between client and server.

### spdlog
[Spdlog](https://github.com/gabime/spdlog) is a lightweight logging library for C++.
This library is used to log all user information and errors to the console.
Furthermore, with the help of the `--debug` argument, debug information can be displayed.

### doxygen
[doxygen](https://www.doxygen.nl) is used to create a HTML documentation based on the docstrings in the source code.

## User Tests (Python)

# Description of System Architecture TODO:
- Description
- Reason
# Interface Description
- Requests and responses are sent using TCP and JSON bodies
  - TCP assures that the request is received
- Messages are published to the client using UDP and JSON bodies
  - In order to reduce overhead, the message is published using UDP
- JSON is used to it's simplicity and readability. It is also a common standard for API communication.

## Client request
```json
{
    "command": int (ENUM)
    "arguments": {
        "{{ ARGUMENT_NAME }}": string,
    }
}
```

command | {{ ARGUMENT_NAME }}
--- | ---
SUBSCRIBE | topicName, clientPort
UNSUBSCRIBE | topicName, clientPort
PUBLISH | topicName, message
LIST_TOPICS | 
GET_TOPIC_STATUS | topicName

**Note**: A client can be started with one ore more topics, but it will call a `SUBSCRIBE` Request for each Topic so its only one Topic per Request

## Server response
### Default
```json
{
    "statusCode": int (ENUM)
}
```

### LIST_TOPIC
```json
{
    "statusCode": int (ENUM),
    "topics": string[]
}
```

### GET_TOPIC_STATUS
```json
{
    "statusCode": int (ENUM),
    "messageTimestamp": uint64_t,
    "subscribers": string[]
}
```

## Server publish (message/heartbeat)
```json
{
    "topic": string,
    "messageTimestamp": uint64_t,
    "message": string
}
```

## TODO:
- Protocols, allowed parameter(area, type), Pre- and Post-Conditions, wrongdoing
# Concurrency
- Multithreading with Mutex

# Source Code Documentation
## Error handling
There is no dedicated error handler.
However, potential error sources that were discovered during development are handled.
Thus, the program will not crash, but the error will be logged using the "error" identifier and the program will continue to run.

## Doxygen

As mentioned before we used doxygen to generate a source code documentation. It can be found in the doc folder and can be opened [here](./doc/html/index.html).

# Testing