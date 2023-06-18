# Publisher / Subscriber System
Abschlusstestat für die Vorlesung Verteilte Systeme and der DHBW Ravensburg im Studiengang Informatik - Informationstechnik des Jahrgangs 2020. <br>
von Johannes Brandenburger, Lukas Braun, Phillip Patzelt, Henry Schuler und Lea Silberzahn

# Table of Contents
- [Scope of Tasks](#Scope-of-Tasks)
- [Technologies](#Instructions)


# Scope of Tasks
<!-- können wir auch Brief overview oder Desription nennen -->
# Installation
# Instructions

# Technologies
- C++
    - JsonCpp
    - spdlog
- Python (User Tests)



# Description of System Architecture
- Description
- Reason
# Interface Description
- Protokolls, allowed parameter(area, type), Pre- and Post-Conditions, wrongdoing
- TCP for the client prompts to the server
- UDP for the Server publish
# Concurrency
- Multithreading with Mutex

# Source Code Dokumentation

# Testing





# OLD

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
DEBUG | --debug | enable logging debug information


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
SUBSCRIBE | topicName, clientPort
UNSUBSCRIBE | topicName
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

## Server publish
```json
{
    "topic": string,
    "messageTimestamp": uint64_t,
    "message": string
}
```