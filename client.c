#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// structure for message queue
struct msg_buffer
{
    long msg_type;
    char msg[1024];
    char hedefClient[512];
} message;

void sendMessage();
void readMessage();
void introduceClientToTheServer();

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Please enter a client id as command line parameter. Ex: ./client jack");
        exit(0);
    }
    char instruction = ' ';
    char *clientID = argv[1];
    char *dummy = "";
    introduceClientToTheServer(clientID);
    while (1)
    {
        printf("Select an operation: \n");
        printf("-Enter 's' to send a message.\n");
        printf("-Enter 'r' to read a message.\n");
        printf("-Enter 't' to terminate the client.\n");
        scanf("%c", &instruction);
        getchar();
        if (instruction == 's')
        {
            sendMessage();
        }
        else if (instruction == 'r')
        {
            readMessage();
        }
        else if (instruction == 't')
        {
            exit(0); // sendde ve readdeki queue acmaları en yukarı alıp, burada da onları msgctl ile kapatabilirsin. sana kalmıs. suan 2 + client sayısı kadarbox acıyor programı her calıstırdıgında. 2 server tanısması.
        }
    }
    return 0;
}

void introduceClientToTheServer(char *clientID)
{
    int pid = getpid();
    char mypid[6];
    sprintf(mypid, "%d", pid);
    char temp[1024] = "";
    strcat(temp, clientID);
    strcat(temp, ",");
    strcat(temp, mypid);
    key_t server_key = ftok("./server.c", 65);
    int msg_id;
    msg_id = msgget(server_key, 0666 | IPC_CREAT);
    message.msg_type = 1;
    strcpy(message.msg, temp);
    msgsnd(msg_id, &message, sizeof(message), 0);
    printf("*The client introduced itself to the server.\n");
}

void sendMessage()
{
    key_t my_key;
    int msg_id;
    my_key = ftok("./client.c", getpid());
    msg_id = msgget(my_key, 0666 | IPC_CREAT);
    message.msg_type = 1;
    printf("*Write the message : \n");
    fgets(message.msg, 1024, stdin);
    printf("*Enter the name of the client to be sent \n");
    fgets(message.hedefClient, 512, stdin);
    msgsnd(msg_id, &message, sizeof(message), 0);
    printf("*Sent message is : %s \n", message.msg);
}
void readMessage()
{
    key_t my_key;
    int msg_id;
    my_key = ftok("./client.c", getpid() - 10000);
    msg_id = msgget(my_key, 0666 | IPC_CREAT);
    msgrcv(msg_id, &message, sizeof(message), 1, 0);
    printf("*Client received the message: %s\n", message.msg);
}