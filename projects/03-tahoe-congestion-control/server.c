#include "header.h"
#define PORT 45525

// Global state so server_send/receive can access them
static int client_fd;
static int last_acked = -1;      // highest in‐order byte sent
static int cwnd;               // congestion window
static int ssthresh;           // slow‐start threshold
static FILE *cwnd_file;        // for writing cwnd.txt

/*
 * @description
 * Write your server's send function here.
 * 1. Send cwnd number of data segments starting with the correct sequence number.
 * 2. Simulate packet loss.
 * 
 * @param
 * You can increase or decrease parameters by yourself.
 * Some useful information can be used either as global varaible or passed as parameters to functions:
 * 1. client_fd: The socket descrip r used for sending/receiving data  
 *               with a client that has connected to server.
 * 2. last_acked: previous acknowledge packets
 * 3. cwnd: congestion window size
*/
void server_send() {
    int seq_start = last_acked + 1;
    int seq_end = last_acked + cwnd;

    for (int seq = seq_start; seq <= seq_end; seq++) {
        // Simulate one forced loss when entering congestion avoidance
        // This helps test cwnd reduction and transition to slow start
        bool loss_flag = (cwnd > ssthresh && seq == seq_start);

        Segment seg = {
            .seq_num = seq,
            .ack_num = 0,
            .loss = loss_flag
        };
        send(client_fd, &seg, sizeof(seg), 0);
        printf("Send: seq_num = %d\n",
               seq);
    }
}


/*
 * @description
 * Write your server's receive function here.
 * We don't need to consider time-out in this lab. Loss will only occur when packet_loss() == true.
 * 1. Receive ACKs from client.
 * 2. Detect if 3 duplicate ACK occurs.
 * 3. Update cwnd and ssthresh.
 * 
 * @param
 * You can increase or decrease parameters by yourself.
 * Some useful information can be used either as global varaible or passed as parameters to functions:
 * 1. client_fd: The socket descriptor used for sending/receiving data  
 *               with a client that has connected to server.
 * 2. last_acked: previous acknowledge packets
 * 3. ssthresh: slow start threshold
 * 4. cwnd: congestion window size
*/
void server_receive() {
    int prev_ack = -1;   // last distinct ACK we saw
    int dup_count = 0;    // how many times in a row we’ve seen prev_ack
    int ack_received = 0;    // most recent ACK value
    int third_dup_ack = -1;   // becomes >=0 once we’ve seen it three times
    int i;

    // 1) pull in up to cwnd ACKs, but break as soon as 3 identical arrive
    for (i = 0; i < cwnd; i++) {
        Segment ack;
        if (recv(client_fd, &ack, sizeof(ack), 0) <= 0) {
            // connection closed or error
            return;
        }
        ack_received = ack.ack_num;
        printf("ACK: ack_num = %d\n", ack_received);

        if (ack_received == prev_ack) {
            dup_count++;
            // two “duplicates” after the first to three identical ACKs total
            if (dup_count == 2) {
                third_dup_ack = ack_received;
                break;
            }
        } else {
            prev_ack  = ack_received;
            dup_count = 0;
        }
    }

    // 2) on the third identical ACK -> fast‐retransmit + reset to slow start
    if (third_dup_ack >= 0) {
        printf("3 duplicate ACKs: ACK_num = %d, ssthresh = %d\n",
               third_dup_ack, ssthresh);

        // cut the window in half & restart slow start
        ssthresh = cwnd / 2;
        cwnd = 1;

        // log and print
        printf("State: slow start (cwnd = %d, ssthresh = %d)\n",
               cwnd, ssthresh);

        // retransmit that un‐ACKed segment immediately
        Segment rexmit = {
            .seq_num = third_dup_ack,
            .ack_num = 0,
            .loss = false
        };
        send(client_fd, &rexmit, sizeof(rexmit), 0);

        // ensure next server_send() starts right
        last_acked = third_dup_ack - 1;
        return;
    }

    // 3) otherwise, no triple‐dup -> regular cwnd growth
    if (cwnd < ssthresh) {
        // slow start
        cwnd *= 2;
    } else {
        // congestion avoidance
        cwnd += 1;
    }

    // 4) slide our “last in‐order” marker
    last_acked = ack_received - 1;

    // 5) log & print the updated cwnd
    if (cwnd < ssthresh) {
        printf("State: slow start (cwnd = %d, ssthresh = %d)\n",
               cwnd, ssthresh);
    } else {
        printf("State: congestion avoidance (cwnd = %d, ssthresh = %d)\n",
               cwnd, ssthresh);
    }
}




/*
 * @description
 * 1. Create a TCP socket bind to port 45525.
 * 2. Listen the TCP socket.
 * 3. Accept the connect and get the client socket file descriptor (client_fd).
 * 4. Send message: "Hi, I'm server {Your_student_ID}" to client.
 * 5. Complete the functions: server_send() and server_receive()
 * 6. Start data transmission with simulating congestion control.
 *
 * To be simple, we receive ACKs only after all cwnd number of data segments are sent, 
 * so we cannot react to 3-duplicate ACKs immediately. This is ok for this lab.
*/
int main(int argc, char* argv[]) {
    srand(time(NULL));
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Initialize congestion control params
    cwnd = 1;
    ssthresh = 8;

    // Open cwnd.txt for writing
    cwnd_file = fopen("cwnd.txt", "w");
    if (!cwnd_file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Create TCP socket.
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    // Set up server's address.

    // Bind socket to the address.
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    // Listening the socket.
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // Accept the connect request.
    client_fd = accept(server_fd,
                       (struct sockaddr *)&client_addr,
                       &addr_len);
    if (client_fd < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("New connection\n");
    fflush(stdout);

    // Send message: "Hi, I'm server {Your_student_ID}" to client.
    const char *greet = "Hi, I'm server 112006265...";
    send(client_fd, greet, strlen(greet) + 1, 0);
    usleep(500000);

    // Start congestion control.
    int ROUND = 10;
    printf("State: slow start (cwnd = %d, ssthresh = %d)\n", cwnd, ssthresh);
    while (ROUND--) {
        // log the cwnd this round
        fprintf(cwnd_file, "%d\n", cwnd);
        fflush(cwnd_file);

        server_send();
        server_receive();
    }

    // Close the socket.
    fclose(cwnd_file);
    printf("Close client socket\n");
    close(client_fd);
    printf("Close server socket\n");
    close(server_fd);
    return 0;
}