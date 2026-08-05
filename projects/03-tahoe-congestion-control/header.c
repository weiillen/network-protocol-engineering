#include "header.h"

/*
 * @description
 * Here are some functions for you to use. 
 * You don't need to modify this part.
*/
bool packet_loss() {
    return ((double) rand() / (double) RAND_MAX) < PACKET_LOSS_RATE;
}

int min(int a, int b) {
    return (a < b) ? a : b;
}
