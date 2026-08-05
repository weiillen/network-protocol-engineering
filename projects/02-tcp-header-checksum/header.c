#include "header.h"

void printHeader(char* header);
void printSegment(Segment* s);
void readFile(Segment* test, char* fileName);

void printHeader(char* header) {
    printf("The header is:\n");
    for (int i = 0; i < 20; i++) {
        printf("%02X ", (uint8_t)header[i]);
    }
    printf("\n");
}

void printSegment(Segment* s) {
    printf("Received information:\n\n\
            Layer 3 information:\n\
            Source IP: %s , Destination IP: %s \n\
            Protocol: %u (TCP)\n\n\
            Layer 4 information:\n\
            Source port: %u , Destination port: %u \n\
            Seq number: %u , Ack number: %u \n\
            Header length: %u (4-bytes) , FLAG: 0x%X (ACK) \n\
            Window size: %u \n\n", s->l3info.SourceIpv4, s->l3info.DestIpv4, s->l3info.protocol,\
            s->l4info.SourcePort, s->l4info.DestPort, s->l4info.SeqNum,\
            s->l4info.AckNum, s->l4info.HeaderLen, s->l4info.Flag, s->l4info.WindowSize);
}

/*
 * @description
 * This function opens a text file for reading.
 *
 * @param
 * test: The testcase of the information for you to create TCP header.
 * fileName: The file name read from sample input. e.g. "sample_input.txt"
*/
void readFile(Segment* test, char* fileName) {
    memset(test, 0, sizeof(Segment));
    char buffer[1024];
    FILE* file = fopen(fileName, "r");

    fgets(buffer, sizeof(buffer), file);
    sscanf(buffer, "%s", test->l3info.SourceIpv4);

    fgets(buffer, sizeof(buffer), file);
    sscanf(buffer, "%s", test->l3info.DestIpv4);

    test->l3info.protocol = 6;   // TCP

    fgets(buffer, sizeof(buffer), file);
    sscanf(buffer, "%u", &test->l4info.SourcePort);

    fgets(buffer, sizeof(buffer), file);
    sscanf(buffer, "%u", &test->l4info.DestPort);

    fgets(buffer, sizeof(buffer), file);
    sscanf(buffer, "%u", &test->l4info.SeqNum);

    fgets(buffer, sizeof(buffer), file);
    sscanf(buffer, "%u", &test->l4info.AckNum);

    test->l4info.HeaderLen = 5;  // data offset
    test->l4info.Flag = 0x10;    // ACK = 1
    
    fgets(buffer, sizeof(buffer), file);
    sscanf(buffer, "%u", &test->l4info.WindowSize);

    fclose(file);
}
