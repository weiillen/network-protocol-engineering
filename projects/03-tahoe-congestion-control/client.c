#include "header.h"
#define PORT 45525

/*
 * @description
 * 1. Create a socket and connect to serverd.
 *    (server's IP address = "127.0.0.1")
 *    (server's Port number = 45525)
 * 2. Receive message: "Hi, I'm server {Your_student_ID}..." from server and print it out.
 *    (The message you sent from server)
 * 3. Continuously receive data from server and send back ACK.
*/
int main(int argc, char *argv[]) {
    int socket_fd;
    struct sockaddr_in server_addr;

    // Create TCP socket.
     socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    // Set up server's address.
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    // Connect to server's socket.
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    // Receive message: "Hi, I'm server {Your_student_ID}..." from server and print it out.
    char buffer[200];
    ssize_t n = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
    if (n < 0) {
        perror("recv");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';
    printf("%s\n", buffer);
    // Receive data and send ACK continuously.
    int expected_seq = 0;
        while (1) {
        Segment seg;
        ssize_t m = recv(socket_fd, &seg, sizeof(seg), 0);
        if (m <= 0) break;  // connection closed

        if (seg.loss) {
            // simulate out‐of‐order arrival → duplicate ACK
            printf("Loss: seq_num = %d\n", seg.seq_num);
            Segment dup_ack = {
                .ack_num = expected_seq,   // same as last in‐order
                .seq_num = 0,
                .loss = false
            };
            if (send(socket_fd, &dup_ack, sizeof(dup_ack), 0) < 0) {
                perror("send");
                break;
            }
            continue;
        }

        // properly received in‐order?  update expected_seq
        printf("Received: seq_num = %d\n", seg.seq_num);
        if (seg.seq_num == expected_seq) {
            expected_seq = seg.seq_num + 1;
        }
        // send fresh ACK
        Segment ack = {
            .ack_num = expected_seq,
            .seq_num = 0,
            .loss = false
        };
        if (send(socket_fd, &ack, sizeof(ack), 0) < 0) {
            perror("send");
            break;
        }
    }

    // Close the socket.
    printf("Close socket\n");
    close(socket_fd);
    return 0;
}