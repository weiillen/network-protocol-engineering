#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct l4info {
    uint32_t SourcePort, DestPort, SeqNum, AckNum, HeaderLen, Flag, WindowSize;
} L4info;

typedef struct l3info {
    char SourceIpv4[16];
    char DestIpv4[16];
    uint32_t protocol;
} L3info;

typedef struct Segment {
    uint8_t pseudoheader[12];
    uint8_t header[20];
    L3info l3info;
    L4info l4info;
} Segment;


void printHeader(char* header);
void printSegment(Segment* s);
void readFile(Segment* test, char* fileName);
