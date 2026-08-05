#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define PACKET_LOSS_RATE 0.1

// In congestion control, we don't need l3 information.
typedef struct Segment {
    int ack_num;
    int seq_num;
    bool loss;
} Segment;

bool packet_loss();
int min(int a, int b);
