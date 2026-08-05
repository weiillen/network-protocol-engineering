#include "header.h"
#define PORT 45525

/*
 * @description
 * In this function, You need to implement the server side
 * that sends messages to and receives messages from the client.
 *
 *         Send the message: "Enter a command (data / time)..."
 * Server -----------------------------------------------------------> Client
 * 
 *         Receive the message: "data" or "time" or {invalid command}
 * Server <----------------------------------------------------------- Client
 * 
 *         data: Read the text file from input and send the segment.
 *         time: Get the current time and send the time string. (format: "2025-01-22 13:14:52")
 *         {invalid command}: Send the message: "Invalid command! Try again! Enter a command (data / time)..."
 * Server -----------------------------------------------------------> Client
 *
 * @param
 * client_fd: The socket file descriptor used for sending/receiving data  
 *            with a client that has connected to server.
 * fileName: The file name read from sample input. e.g. "sample_input.txt"
*/

void serverFunction(int client_fd, char* fileName) {
    char cmd[20], buf[70];
    int n;
    int first = 1;

    while (1) {
        /* prompt (first time) or invalid message thereafter */
        if (first) {
            send(client_fd,
                 "Enter a command (data / time)...\n",
                 strlen("Enter a command (data / time)...\n"),
                 0);
            first = 0;
        } else {
            send(client_fd,
                 "Invalid command! Try again! Enter a command (data / time)...\n",
                 strlen("Invalid command! Try again! Enter a command (data / time)...\n"),
                 0);
        }

        /* read client’s response */
        n = recv(client_fd, cmd, sizeof(cmd)-1, 0);
        if (n <= 0) return;      // client closed or error
        cmd[n] = '\0';

        /* data request */
        if (strncmp(cmd, "data", 4) == 0) {
            Segment seg;
            readFile(&seg, fileName);
            send(client_fd, &seg, sizeof(seg), 0);
            return;
        }
        /* time request */
        else if (strncmp(cmd, "time", 4) == 0) {
            time_t t = time(NULL);
            struct tm *tmv = localtime(&t);
            snprintf(buf, sizeof(buf),
                     "%04d-%02d-%02d %02d:%02d:%02d\n",
                     tmv -> tm_year + 1900,
                     tmv -> tm_mon + 1,
                     tmv -> tm_mday,
                     tmv -> tm_hour,
                     tmv -> tm_min,
                     tmv -> tm_sec);
            send(client_fd, buf, strlen(buf), 0);
            return;
        }
        /* otherwise: loop and re-prompt */
    }
}

/*
 * @description
 * 1. Create a TCP socket bind to port 45525.
 * 2. Listen the TCP socket.
 * 3. Accept the connect and get the client socket file descriptor (client_fd).
 * 4. Send message: "Hi, I'm server {Your_student_ID}" to client.
 * 5. Complete the function: serverFunction()
*/
int main(int argc, char* argv[]) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    // Create TCP socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Set up server's address.
    struct sockaddr_in srv = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    // Allow immediate reuse of the port after we close the server
    {
        int yes = 1;
        if (setsockopt(server_fd,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       &yes,
                       sizeof(yes)) < 0) {
            perror("setsockopt");
            exit(1);
        }
    }

    // Bind socket to the address.

   if (bind(server_fd,
             (struct sockaddr*)&srv,
             sizeof(srv)) < 0) {
        perror("bind failed");
        exit(1);
    }

    // Listening the socket.

    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        exit(1);
    }

    printf("New connection\n");


    while(1) {
        // Accept the connect request.
        // client_fd = ...
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);
        int client_fd = accept(server_fd,
                               (struct sockaddr*)&cli,
                               &len);
        if (client_fd < 0) continue;
        
        // Send message: "Hi, I'm server {Your_student_ID}" to client.
        {
            char greet[70];
            snprintf(greet, sizeof(greet),
                     "Hi, I'm server 112006265");
            send(client_fd, greet, strlen(greet), 0);
        }

        // Send messages to client.
        usleep(500000);
        serverFunction(client_fd, argv[1]);
        close(client_fd);
    }

    return 0;
}
