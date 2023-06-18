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
- Zur Überprüfung der Funktionalität des Systems wurden User-Tests mithilfe von Python durchgeführt. 
  - Hierbei werden die Ausgaben des Clients in separaten Dateien geschrieben und mit den erwarteten Ausgaben verglichen.
    - Die Datei **subscribe_client_with_one_topic.txt** enthält die Ausgaben des Clients, wenn dieser sich nur bei einem Topic subscribed. Zusätzlich kann in dieser Datei der Heartbeat des Clients beobachtet werden. Der Subscribe wird  mit den folgenden Codezeilen durchgeführt:
        ```
        subscribe_client_process_with_one_topic = subprocess.Popen([client_path, "--subscribe", "ErstesTopic", "--server-port", "8080"], stdin=subprocess.PIPE, stdout=subscribe_client_with_one_topic, stderr=subprocess.STDOUT)
        ```	
    - Die Datei **subscribe_client_with_multiple_topics.txt** enthält die Ausgaben des Clients, wenn dieser sich bei mehreren Topics subscribed. Hierbei wird probiert sich auf das gleiche Topic mehrmals zu subscriben. Der erste Subscribe-Versuch sollte erfolgreich sein, die weiteren Versuche sollten fehlschlagen. Zusätzlich kann in dieser Datei ebenfalls der Heartbeat des Clients beobachtet werden. Der Subscribe wird mit den folgenden  Codezeilen durchgeführt:
        ```
        subscribe_client_process_with_two_topics = subprocess.Popen([client_path, "--subscribe", "ErstesTopic", "ErstesTopic", "--server-port", "8080"], stdin=subprocess.PIPE, stdout=subscribe_client_with_two_topics, stderr=subprocess.STDOUT)
        ```	
    - Die Datei **execute_commands_client_output_file.txt** enhtält die Ausgaben von mehreren Commands, die auf dem Client ausgeführt wurden. Hierbei wird das **publishen** bei vorhandenen und nicht vorhandenen Topics getestet. 
    Zusätzlich wird das Ausführen des Commands **list-topics** getestet. Zuletzt wird der Command **get-topic-status** getestet. Hierbei wird der Status von vorhandenen und nicht vorhandenen Topics abgefragt. Die Commands werden mit den folgenden Codezeilen durchgeführt:
        ```
        # Commands to Test
        client_commands = [
            ["../../client.exe", "--publish", "ErstesTopic", "Message", "--server-port", "8080"],
            ["../../client.exe", "--publish", "NichtExistierendesTopic", "Message", "--server-port", "8080"],
            ["../../client.exe", "--list-topics", "--server-port", "8080"],
            ["../../client.exe", "--get-topic-status", "ErstesTopic", "--server-port", "8080"],
            ["../../client.exe", "--get-topic-status", "NichtExistierendesTopic", "--server-port", "8080"]
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