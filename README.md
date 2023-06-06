# Publisher-Subscriber-System
Abschlusstestat für die Vorlesung Verteilte Systeme von Brandenburger, Braun, Patzelt, Schuler und Silberzahn

# Instructions

## Start the Server
A server is started with the following arguments:
```
--port (optional)
    Port to listen on
    Default: 8080
--topic-timeout (optional in Seconds) [default 30]
    Inactivity Timeout in seconds for all topics. If there are no new mesages over this time each subscriber gets a wakeup message.
    Default: 30
```

There is a `--help` command to display all available arguments in the terminal as well.

## Start the Client
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


# JSON communication

## Client request
```json
{
    "command": int (ENUM)
    "arguments": {
        "{{ ARGUMENT_NAME }}": string,
    }
}
```

ID | Command | {{ ARGUMENT_NAME }} | Description | Further Information
--- | --- | --- | --- | ---
0 | subscribe | topicName, clientPort | Registers the subscriber on the topic. If the topic does not exist, it is created. | 
1 | unsubscribe | topicName | Unsubscribes the subscriber from the topic. If no more subscribers are registered on this topic, it will be deleted. | *Topic does not exist → error* <!--TODO:-->
2 | publish | topicName, message | Publishes a new message on the topic. | *Topic does not exist → error* <!--TODO:-->
3 | listTopics | - | Requests a list of available topics. |
4 | getTopicStatus | topicName | Returns the current status of a topic. |

Argument | Type | Value Range, ...
--- | --- | ---
topicName | string | 1-255 characters
clientPort | string | 0-65535 (integer)
message | string | 1-255 characters 

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

## Server publish
```json
{
    "topic": string,
    "messageTimestamp": uint64_t,
    "message": string
}
```

## ENUMs

```c++
enum CommandIdentifiers {
    subscribe = 0,
    unsubscribe = 1,
    publish = 2,
    listTopics = 3,
    getTopicStatus = 4
};
```

```c++
enum CommunicationParameters {
    topicName = 0,
    clientPort = 1,
    message = 2,
    topics = 3,
    messageTimestamp = 4,
    subscribers = 5
};
```

```c++
enum Statuscode {
    success = 0,
    failed = 1,
    invalidParameter = 2,
    internalError = 3,
};
```

# Architectural Description with Design Decision Explanation

<!--TODO:-->

# Tests

The project has been tested with automated user tests.
