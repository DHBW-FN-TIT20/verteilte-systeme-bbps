import time
import subprocess

server_path = "../../server.exe"
server_process = subprocess.Popen([server_path])

# Some Delay
time.sleep(2)

output_file = open("output.txt", "w")

# Commands 
# SUBSCRIBE | topicName, clientPort
# PUBLISH | topicName, message
# LIST_TOPICS | 
# GET_TOPIC_STATUS | topicName
client_commands = [
    ["../../client.exe", "--subscribe", "ErstesTopic", "--port", "0"],
    ["../../client.exe", "--publish", "ErstesTopic", "Message"],
    ["../../client.exe", "--list-topics"],
    ["../../client.exe", "--get-topic-status", "ErstesTopic"]
]

for command in client_commands:
    # Start client and redirect input/output to the log file
    client_process = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=output_file, stderr=subprocess.STDOUT, universal_newlines=True)

    # Execute command in client
    client_process.communicate()
    client_process.wait()

    # Some Delay
    time.sleep(2)

server_process.terminate()
output_file.close()