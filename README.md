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

command | {{ ARGUMENT_NAME }}
--- | ---
subscribe | topicName, clientPort
unsubscribe | topicName
publish | topicName, message
listTopics | 
getTopicStatus | topicName

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



# Tests

The project has been tested with automated interface tests.
The tests can be found in the `src/tests/` folder and can be executed with `make test`.
A server and a client are initialized and their functionality is tested with predefined JSON requests.

- `test_server.cpp` tests the functionality of the server
  - ...
- `test_client.cpp` tests the functionality of the client
  - ...

