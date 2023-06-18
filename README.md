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
    - The file **execute_commands_client_output_file.txt** contains the outputs of multiple commands executed on the client. This tests the **publishing** on both existing and non-existing topics. If a topic has multiple subscribers, all of them will be notified almost simultaneously when a publish action occurs. The following status codes should be returned:
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