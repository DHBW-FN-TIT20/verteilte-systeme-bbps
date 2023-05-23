# verteilte-systeme-bbpss
Abschlusstestat für die Vorlesung Verteilte Systeme von Brandenburger, Braun, Patzelt, Schuler und Silberzahn


# Instructions

## Starting the Server
A server is started with the following arguments:
```
--port (optional)
    Port to listen on
    Default: 8080
--topic-timeout (optional in Seconds) [default 30]
    Inactivity Timeout in seconds for all topics. If there are no new mesages over this time each subscriber gets a wakeup message.
    Default: 30
```

There is offered also a `--help` command to display this in Commandline