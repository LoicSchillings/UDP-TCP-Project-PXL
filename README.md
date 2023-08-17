# UDP-TCP-Project-PXL
In this project I was tasked to establish a UDP connection and a TCP connection between a custom C program for a server and client for both protocols.

In the UDP connection, the server would send a series of random integers to the client.
The client had to find the largest integer and return it to the server.
This process would happen twice before the connection would be terminated.

In the TCP connection, the client would generate a random amount of random and simple mathmetic operations.
The server was tasked to calculate the solutions of those operations, and send them back to the client.
To tell the server all calculations were executed, the client would send STOP to the server and the connection would be terminated.
