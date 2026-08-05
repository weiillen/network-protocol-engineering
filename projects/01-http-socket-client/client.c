#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 80  // Define the port number (80 for HTTP)
// for receiving
#define BUFFER_SIZE 8000 

int main(int argc, char* argv[]) {
    int socket_fd;
    struct sockaddr_in server_addr;
    char request[] = "GET / HTTP/1.1\r\nHost: motherfuckingwebsite.com\r\n\r\n";
    // for receiving
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    // Flag to track if we found any <h2> content
    int h2_found = 0;  

    // Create a TCP socket.
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }


    // Set up the server's address.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    struct hostent* h = gethostbyname("motherfuckingwebsite.com");
    if (h == NULL) {
        perror("Failed to resolve hostname");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    memcpy(&server_addr.sin_addr, h->h_addr_list[0], h->h_length);

    // Connect to the server.
    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    
    // Send the HTTP GET request.
    if (send(socket_fd, request, strlen(request), 0) < 0) {
        perror("Send failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // Receive the server's response.
    // You might use while loop to receive packets.
    while ((bytes_received = recv(socket_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';  // Null-terminate the buffer
        // printf("%s", buffer);  // Print the response

        // Print out the words with label <h2></h2>.
        char *h2_start = buffer;
        while ((h2_start = strstr(h2_start, "<h2>")) != NULL) {
            char *h2_end = strstr(h2_start, "</h2>");
            if (h2_end) {
                // We found at least one <h2> tag
                h2_found = 1;
                // Temporarily terminate the string
                *h2_end = '\0';  
                // +4 to skip "<h2>"
                printf("%s\n", h2_start + 4);  
                // Restore the original character
                *h2_end = '<';  
                // Continue searching after this tag
                h2_start = h2_end + 1;  
            } else {
                // No closing tag found
                break;  
            }
        }
    }

    // Close the socket.
    close(socket_fd);

    return 0;
}