#include "header.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 45525

// TCP pseudo-header
/*
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                         Source Address                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                      Destination Address                      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|      Zero     |    Protocol   |           TCP Length          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

// TCP header (without options)
/*
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Source Port          |       Destination Port        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                        Sequence Number                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                     Acknowledgment Number                     |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Data |           |U|A|P|R|S|F|                               |
| Offset|  Reserved |R|C|S|S|Y|I|          Window Size          |
|       |           |G|K|H|T|N|N|                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
+            Checksum           |      Urgent Data Pointer      +
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

/*
 * @description
 * 1. Create pseudo-header
 * 2. Create header without checksum
 * 3. Calculate the checksum
 * 4. Fill in the checksum field in the header
 * 
 * @param
 * s: The segment sent from server
*/
// Assume that the TCP segment is a pure ACK (no payload), that is TCP_length == TCP_header_length.
// Note that TCP_header_length is a multiple of 4 bytes.
// Hint: memcpy(), inet_addr(), htons(), htonl(), uint16_t, uint32_t, bitwise operation
void createHeader(Segment* s) {
    uint8_t *ph = s->pseudoheader;
    uint8_t *th = s->header;
    uint32_t sum = 0;
    uint16_t w;

    // build pseudo‑header 
    uint32_t sip = inet_addr(s->l3info.SourceIpv4);
    uint32_t dip = inet_addr(s->l3info.DestIpv4);
    memcpy(&ph[0], &sip, 4);
    memcpy(&ph[4], &dip, 4);
    ph[8] = 0;
    ph[9] = (uint8_t)s->l3info.protocol;
    *(uint16_t*)&ph[10] = htons(s->l4info.HeaderLen * 4);

    // zero‑out checksum, fill rest of TCP header 
    memset(th, 0, 20);
    *(uint16_t*)&th[0]  = htons((uint16_t)s->l4info.SourcePort);
    *(uint16_t*)&th[2]  = htons((uint16_t)s->l4info.DestPort);
    *(uint32_t*)&th[4]  = htonl(s->l4info.SeqNum);
    *(uint32_t*)&th[8]  = htonl(s->l4info.AckNum);
    th[12] = (s->l4info.HeaderLen << 4) & 0xF0;
    th[13] = (uint8_t)s->l4info.Flag;
    *(uint16_t*)&th[14] = htons((uint16_t)s->l4info.WindowSize);

    // sum pseudo‑header
    for (int i = 0; i < 12; i += 2) {
        w = (ph[i] << 8) | ph[i+1];
        sum += w;
    }
    // sum TCP header (skip checksum bytes at offset 16)
    for (int i = 0; i < 20; i += 2) {
        if (i == 16) continue;
        w = (th[i] << 8) | th[i+1];
        sum += w;
    }
    // fold carries 
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    // complement
    uint16_t cs = ~sum;
    th[16] = cs >> 8;
    th[17] = cs & 0xFF;

    /*
    // For debugging
    printf("The pseudo-header is:\n");
    for (int i = 0; i < 12; i++) {
        printf("%02X ", (uint8_t)(s->pseudoheader[i]));
    }
    printf("\n");
    */
}

/*
 * @description
 * In this function, You need to implement the client side
 * that sends messages to and receives messages from the server.
 *
 *         Receive the message: "Enter a command (data / time)..."
 * Server -----------------------------------------------------------> Client
 * 
 *         Send the message: "data" or "time" or {invalid command}
 * Server <----------------------------------------------------------- Client
 * 
 *         data: 1. Print "Receive DATA from server..."
 *               2. Receive Segment from server
 *               3. printSegment() -> createHeader() -> printHeader()
 *               4. Close the socket
 *         time: 1. Print "Receive TIME from server..."
 *               2. Receive time string from server and print it out
 *               3. Close the socket
 *         {invalid command}: Do nothing
 * Server -----------------------------------------------------------> Client
 *
 * @param
 * socket_fd: The socket file descriptor used for sending/receiving data
 *            with a client that has connected to server.
 * s        : The segment sent from server
*/
void receiveData(int socket_fd, Segment* s) {
   char buf[200], cmd[20];
    int n;

    // get prompt
    if ((n = recv(socket_fd, buf, sizeof(buf)-1, 0)) <= 0) return;
    buf[n] = '\0';
    
    printf("server: %s", buf);

    while (1) {
        if (!fgets(cmd, sizeof(cmd), stdin)) return;
        cmd[strcspn(cmd, "\n")] = '\0';

        send(socket_fd, cmd, strlen(cmd), 0);

        // If 'data' command
        if (strcmp(cmd, "data") == 0) {
            printf("Receive DATA from server...\n\n");
            recv(socket_fd, s, sizeof(*s), 0);
            printSegment(s);
            createHeader(s);
            printHeader((char*)s->header);

            printf("\n");

            return;
        }

        // If 'time' command
        else if (strcmp(cmd, "time") == 0) {
            printf("Receiving TIME from server...\n\n");
            if ((n = recv(socket_fd, buf, sizeof(buf)-1, 0)) > 0) {
                buf[n] = '\0';
                printf("%s\n", buf);
            }
            return;
        }

        // Invalid command
        if ((n = recv(socket_fd, buf, sizeof(buf)-1, 0)) <= 0) return;
        buf[n] = '\0';
        printf("server: %s", buf);
    }
}

/*
 * @description
 * 1. Create a socket and connect to server.
 *    (server's IP address = "127.0.0.1")
 *    (server's port number = 45525)
 * 2. Receive message: "Hi, I'm server {Your_student_ID}..." from server and print it out.
 *    (The message you sent from server)
 * 3. Complete the function: receiveData()
*/
int main(int argc, char* argv[]) {
    int socket_fd;
    struct sockaddr_in server_addr;

    // Create TCP socket.
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket");
        return 1;
    }


    // Set up server's address.
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);


    // Connect to server's socket.
    if (connect(socket_fd,
               (struct sockaddr*)&server_addr,
               sizeof(server_addr)) < 0) {
       perror("connect failed");
       close(socket_fd);
       return 1;
   }

    // Receive message: "Hi, I'm server {Your_student_ID}..." from server and print it out.
     {
        char welcome[200];
        int n = recv(socket_fd, welcome, sizeof(welcome)-1, 0);
        if (n > 0) {
            welcome[n] = '\0';
             printf("server: %s\n", welcome);
        }
    }

    Segment s;
    receiveData(socket_fd, &s);

    printf("Close socket\n");
    close(socket_fd);
    return 0;
}
