import time
import subprocess

server_path = "../../server.exe"
server_process = subprocess.Popen([server_path, "--port", "8080"])

# Some Delay
time.sleep(5)

#Output Files
subscribe_client_with_two_topics = open("subscribe_client_with_two_topics.txt", "w")
subscribe_client_with_one_topic = open("subscribe_client_with_one_topic.txt", "w")
execute_commands_client_output_file = open("execute_commands_client_output_file.txt", "w")

#Client Path
client_path = "../../client.exe"

subscribe_client_process_with_two_topics = subprocess.Popen([client_path, "--subscribe", "ErstesTopic", "ErstesTopic", "--server-port", "8080"], stdin=subprocess.PIPE, stdout=subscribe_client_with_two_topics, stderr=subprocess.STDOUT)
subscribe_client_process_with_one_topic = subprocess.Popen([client_path, "--subscribe", "ErstesTopic", "--server-port", "8080"], stdin=subprocess.PIPE, stdout=subscribe_client_with_one_topic, stderr=subprocess.STDOUT)

time.sleep(5)
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

#Closing Output Files
execute_commands_client_output_file.close()
subscribe_client_with_two_topics.close()
subscribe_client_with_one_topic.close()
