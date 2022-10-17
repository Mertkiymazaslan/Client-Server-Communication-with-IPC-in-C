Goal of the project: To ensure a communication between different clients using server.(when a client wants to send a message to another client,
it goes to the server, then the worker thread in the server receives the message and sends the message to the target client.

* C program for client-server communication using Inter Process Communication with message queues and multithreading in server.
For every client, there are a working thread in server.

How to run?
-Run the server first. ( ./server )
-Then, create a number of clients by running "client.c".
-When creating a client, you must give it a unique id as a argv parameter. ( example: ./client jack )
-Please read the message right after sending it. Do not send more than one message to the same client without reading first.
Rest is easy to understand if you follow the commands I printed on the screen.
