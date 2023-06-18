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
- [Description of System Architecture](#description-of-system-architecture)
- [Interface Description](#interface-description)
  - [Client request](#client-request)
  - [Server response](#server-response)
    - [Default](#default)
    - [LIST\_TOPIC](#list_topic)
    - [GET\_TOPIC\_STATUS](#get_topic_status)
    - [Server publish (message/heartbeat)](#server-publish-messageheartbeat)
  - [Wrongdoing](#wrongdoing)
    - [Subscribe](#subscribe)
    - [Unsubscribe](#unsubscribe)
    - [Publish](#publish)
    - [ListTopics](#listtopics)
    - [GetTopicStatus](#gettopicstatus)
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
- Client and Server (C++)
  - [JsonCpp](https://github.com/open-source-parsers/jsoncpp) is used to work with JSON structures in C++. This library is used to parse the request and response bodies for the communication between client and server.
  - [Spdlog](https://github.com/gabime/spdlog) is a lightweight logging library for C++. This library is used to log all user information and errors to the console. Furthermore, with the help of the `--debug` argument, debug information can be displayed.
  - [doxygen](https://www.doxygen.nl) is used to create a HTML documentation based on the docstrings in the source code.
- User Tests (Python)
  - To test the application a simple python script starts the client and the server and performs operations just like a user. The script than compares the actual output with the expected output to ensure the correct working.

# Description of System Architecture
![Architecture](doc/diagrams/architecture.svg)
- There are two main components: the Client and the Server class
- Topic management on the server side is done by the Topic class
  - The Topic class stores the subscribers and the last message
- The ClientConnection class is used to manage the communication between the server and the client (on the server side)
  - For each client only one ClientConnection object is created
  - For the communication, the Command, Message and Response classes are introduced which act as a blueprint for the JSON bodies.
    - The classes provide a JSON serializer and deserializer as well as a structured way to store and access the data.
- In order to improve readability, three Enums are created: StatusCode, CommunicationIdentifiers and CommunicationParameters

# Interface Description
- Requests and responses are sent using **TCP** and JSON bodies
  - TCP assures that the request is received
- Messages are published to the client using **UDP** and JSON bodies
  - In order to reduce overhead, the message is published using UDP
- JSON is used due to it's simplicity and readability. It is also a common standard for API communication.

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

### Server publish (message/heartbeat)
```json
{
    "topic": string,
    "messageTimestamp": uint64_t,
    "message": string
}
```
## Wrongdoing
Every response contains a status code.
If a request does not meet the specified JSON format, a Internal Error Status Code is returned.
### Subscribe
If the Topic doesn't exist a new topic will be generated
### Unsubscribe
Returns an Invalid Parameters Status Code, if the Topic does not exist.
### Publish
Returns an Invalid Parameter Status Code, if the topic does not exist.
### ListTopics
Returns an empty list if no topics exists.
### GetTopicStatus
Returns an Invalid Parameter Status Code, if the topic does not exist.


# Concurrency
Concurrency is implemented using threads.
On the client side, a separate thread is created for handling incoming UDP packages containing new published messages from the server, which are further logged to the console.
Since this task does not require accessing shared resources, no mutex is required.

To ensure the concurrency of the server however, multithreading is used in conjunction with a mutex. 
The server creates a thread for each incoming client to handle their requests. 
Additionally, a heartbeat thread is running to check the timestamps and therefore the topic timeout.

# Source Code Documentation
## Error handling
There is no dedicated error handler.
However, potential error sources that were discovered during development are handled.
Thus, the program will not crash, but the error will be logged using the "error" identifier and the program will continue to run.

## Doxygen
As mentioned before we used doxygen to generate a source code documentation. It can be found in the doc folder and can be opened [here](./doc/html/index.html).

# Testing
- User tests were conducted using Python to verify the functionality of the system.
  - In these tests, the outputs of the clients are written to separate files and compared with the expected outputs.
    - The file **subscribe_client_with_one_topic.txt** contains the outputs of the client when it subscribes to only one topic. Additionally, the heartbeat of the client can be observed in this file. The subscription is performed using the following lines of code:
        ```
        subscribe_client_process_with_one_topic = subprocess.Popen([client_path, "--subscribe", "FirstTopic", "--server-port", "8080"], stdin=subprocess.PIPE, stdout=subscribe_client_with_one_topic, stderr=subprocess.STDOUT)
        ```	
      To verify that the command was executed successfully, the file should contain the following output:
      - **Status: Success**
    - The file **subscribe_client_with_multiple_topics.txt** contains the outputs of the client when it subscribes to multiple topics. In this case, an attempt is made to subscribe to the same topic multiple times. The first subscription attempt should be successful, while the subsequent attempts should fail. Additionally, the heartbeat of the client can also be observed in this file. The subscription is performed using the following lines of code:
        ```
        subscribe_client_process_with_two_topics = subprocess.Popen([client_path, "--subscribe", "FirstTopic", "SecondTopic", "--server-port", "8080"], stdin=subprocess.PIPE, stdout=subscribe_client_with_two_topics, stderr=subprocess.STDOUT)
        ```	
        When subscribing to the topic for the first time, the file should return **Status: Success** as the first status code. The second attempt should return **Status: Failed** as the status code.
    - The file **execute_commands_client_output_file.txt** contains the outputs of multiple commands executed on the client. This tests the **publishing** on both existing and non-existing topics. If a topic has multiple subscribers, all of them will be notified almost simultaneously when a publish action occurs (e.g.: [file1](./src/test/subscribe_client_with_one_topic.txt) line 11, [file2](./src/test/subscribe_client_with_two_topics.txt) line 16). The following status codes should be returned:
        - **Status: Success** (when publishing to the topic for the first time)
        - **Status: Failed** (when publishing to the topic for the second time)
    - Additionally, the execution of the **list-topics** command is tested. 
    In this case, the following information should be returned:
      - **Status: Success** and **Topics: ErstesTopic** (when publishing to the topic for the first time)
    - Finally, the **get-topic-status** command is tested, which retrieves the status of existing and non-existing topics. The following information should be returned:
      - **Status: Success**, **Message-Timestamp:** und **Subscriber:** (when publishing to the topic for the first time)
      - **Status: Invalid Paramerter** (when using get-topic-status on a non-existing topic)
    The commands are executed using the following lines of code:
        ```
        # Commands to Test
        client_commands = [
            ["../../client.exe", "--publish", "FirstTopic", "Message", "--server-port", "8080"],
            ["../../client.exe", "--publish", "NonExistingTopic", "Message", "--server-port", "8080"],
            ["../../client.exe", "--list-topics", "--server-port", "8080"],
            ["../../client.exe", "--get-topic-status", "FirstTopic", "--server-port", "8080"],
            ["../../client.exe", "--get-topic-status", "NonExistingTopic", "--server-port", "8080"]
        ]

        for command in client_commands:
            # Start client and redirect input/output to the log file
            client_process = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=execute_commands_client_output_file, stderr=subprocess.STDOUT)
            # Execute command in client
            client_process.communicate()
            client_process.wait()

            # Some Delay
            time.sleep(5)
        ```
