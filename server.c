#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
// Define message queue structure

struct msg_buffer
{
    long msg_type;
    char msg[1024];
    char hedefClient[512];
} message;

char clientIDs[20][300];  // daha önce tanıtılan clientlerin id leri.
char processIDs[20][300]; // daha önce tanıtılan clientlerin process id leri.
int clientAndProcessIDsIndex = 0;

void *workerThread(void *vargp)
{
    printf("*A new worker thread opened to communicate with the added client.\n");
    key_t my_key;
    int msg_id;
    my_key = ftok("./client.c", atoi((char *)vargp));
    msg_id = msgget(my_key, 0666 | IPC_CREAT);

    while (1)
    {
        msgrcv(msg_id, &message, sizeof(message), 1, 0);
        printf("*Worker thread received the message: %s\n", message.msg);
        // msj alındı, smdi gonderilecek.
        printf("*Worker thread is sending this message to the client: %s\n", message.hedefClient);
        int size = strlen(message.hedefClient);
        message.hedefClient[size - 1] = '\0';
        int idx = 0;
        while (strcmp(message.hedefClient, clientIDs[idx]) != 0) // clientID sine denk gelen process id sini buluyoruz.
        {
            idx++;
            if (idx > clientAndProcessIDsIndex + 1)
            {
                printf("There are no client named %s. Server terminated!\n", message.hedefClient);
                exit(0);
            }
        }
        // simdi gönderilecek process id ile bir mailbox ac ve gönder.
        key_t my_key1;
        int msg_id1;
        my_key1 = ftok("./client.c", atoi(processIDs[idx]) - 10000); // unique key
        msg_id1 = msgget(my_key1, 0666 | IPC_CREAT);
        message.msg_type = 1;
        msgsnd(msg_id1, &message, sizeof(message), 0);
        printf("*Worker thread sent the message: %s \n", message.msg);
    }

    return NULL;
}
int main()
{
    // printf("girdi1"); // bunu neden okumuyor serveri acınca??????
    while (1)
    {
        key_t my_key;
        int msg_id;
        my_key = ftok("./server.c", 65);
        msg_id = msgget(my_key, 0666 | IPC_CREAT);
        msgrcv(msg_id, &message, sizeof(message), 1, 0);
        msgctl(msg_id, IPC_RMID, NULL);           // parcalıyosun
        printf("Server saved the new client.\n"); // server clientID yi ve clientin process id sini paralel olarak 2 arrayda tutuyor.
        // first introduce -- eger first introduce ise main threade gitsin, yoksa worker thread ile normal mesajlaşmasını yapıcak.
        char clientID[300];
        char processID[300];
        char *ptr = strtok(message.msg, ",");
        strcpy(clientID, ptr);
        int i = 0;
        while (ptr != NULL)
        {
            ptr = strtok(NULL, ",");
            if (i == 0)
                strcpy(processID, ptr);
            i++;
        }
        strcpy(clientIDs[clientAndProcessIDsIndex], clientID);   // save new client id to the array.
        strcpy(processIDs[clientAndProcessIDsIndex], processID); // save new process id to the array. Bu arraylari daha sonra worker threadde mesajı gönderirken client ID sinden process ID sine gecip, oradan unique bir mailbox keyi olusturmak icin kullanıyorum.
        clientAndProcessIDsIndex++;
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, workerThread, processID);
    }

    return 0;
}