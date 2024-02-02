#include <sys/socket.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <poll.h>
#include <limits.h>
#include "factor.h"



void sendCharMessage(int sockfd, uint8_t message, int* flag) {
    int bsent = send(sockfd, &message, 1, 0);
      if (bsent < 0) {
          printf("Error sending to the socket.");
          *flag = 1;
    }
}

void sendShortMessage(int sockfd, uint16_t message, int* flag) {
    message = htons(message);
    int bsent = send(sockfd, &message, 2, 0);
      if (bsent < 0) {
          printf("Error sending to the socket.");
          *flag = 1;
    }
}

void receiveCharMessage(int sockfd, uint8_t *message, int* flag) {
    int brec = recv(sockfd, message, 1, 0);
        if (brec < 0) {
            printf("Error receiving to the socket.");
            *flag = 1;
        } else if (*message > 0x06) {
            printf("Error: Message doesn't exist");
            *flag = 1;
        }
}
void receiveShortMessage(int sockfd, uint16_t* message, int* flag) {
    int brec = recv(sockfd, message, 2, 0);
        if (brec < 0) {
            printf("Error receiving to the socket.");
            *flag = 1;
        }
    *message = ntohs(*message);
}
void receiveListElement(int sockfd, uint32_t *listEl, int* flag) {
    int brec = recv(sockfd, listEl, 4, 0);
        if (brec < 0) {
            printf("Error receiving to the socket.");
            *flag = 1;
        }
    *listEl = ntohl(*listEl);
}


int main(int argc, char** argv) {
    if ( argc < 2 ) {
        fprintf(stderr, "Usage: %s <port>\n",argv[0]);
        return 1;
    }
    // 1. Create a socket.
    int sockfd;
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if ( sockfd < 0 ) {
        fprintf(stderr, "Socket creation failed with error %d\n", errno);
        return 1;
    }
    int e;
    // 2. Bind a port to the socket.
    uint16_t port = (uint16_t)strtoul(argv[1], NULL, 10);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((port));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if ( e < 0 ) {
    fprintf(stderr, "Binding port %d failed with error %d\n", port, errno);
    return 1;
    }
    // 3. Configure the socket to listen for new connections.
    e = listen(sockfd, 5);
    if ( e < 0 ) {
        fprintf(stderr, "Listening failed with error %d\n", errno);
        return 1;
    }
    while ( 1 ) {
        // 4. Accept a new connection.
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int clientfd = accept(sockfd,
        (struct sockaddr*)(&client_addr),
        &client_addr_len);
        if ( clientfd < 0 ) {
            fprintf(stderr, "Accept failed with error %d\n", errno);
            return 1;
        }
        // 5. Exchange data with the client on the new socket.
        int flag = 0;

        // Receive message 01 to connect to the client
        uint8_t message;
        receiveCharMessage(clientfd, &message, &flag);
        if (flag) {
            return 1;
        }
        // Receive contents of message 01 from the client
        uint16_t contents;
        receiveShortMessage(clientfd, &contents, &flag);
        if (flag) {
            return 1;
        }
        printf("Receiving the message 01, and message's content, %x\n", message);
        
        // Send message 02 from the server
        message = 0x02;
        sendCharMessage(clientfd, message, &flag);
        if (flag) {
            return 1;
        }
        // Send the contents (client id) of message 02
        uint16_t u16_client_id = 111;
        sendShortMessage(clientfd, u16_client_id, &flag);
        if (flag) {
            return 1;
        }
        printf("Sending the message 02, and message's content, %d\n", message);

        //Receive message 03 to find work 
        receiveCharMessage(clientfd, &message, &flag);
        if (flag) {
            return 1;
        }
        // Receive the contents (client id) for work
        receiveShortMessage(clientfd, &u16_client_id, &flag);
        if (flag) {
            return 1;
        }
        printf("Receiving the message 03, and message's content, %x\n", message);
        
        // Send message 04
        message = 0x04;
        sendCharMessage(clientfd, message, &flag);
        if (flag) {
            return 1;
        }
        // Send contents of message 04 (work to do)
        uint32_t workToDo = 1000;
        workToDo = htonl(workToDo);
        int bsent = send(clientfd, &workToDo, 4, 0);
        if (bsent < 0) {
            printf("Error sending to the socket.");
            return 1;
        }
        printf("Sending the message 04, and message's content is %d\n", workToDo);

        // Receive message 03 to find work AGAIN
        receiveCharMessage(clientfd, &message, &flag);
        if (flag) {
            return 1;
        }
        // Receive the contents of 03 (client id) for work AGAIN
        receiveShortMessage(clientfd, &u16_client_id, &flag);
        if (flag) {
            return 1;
        }
        printf("Receiving the message 03, and message's content, %x\n", message);

        // Send message 05 to say that we send the work to do but haven't received results yet
        message = 0x05;
        sendCharMessage(clientfd, message, &flag);
        if (flag) {
            return 1;
        }
        printf("Sending the message 05, and message's content is none\n");

        // Receive message 06
        receiveCharMessage(clientfd, &message, &flag);
        if (flag) {
            return 1;
        }
        // Receive contents of message 06 (work done)
        receiveShortMessage(clientfd, &u16_client_id, &flag);        // Receiving client id 
        if (flag) {
            return 1;
        }
        printf("Receiving the message 06, and message's content, %x\n", u16_client_id);
        uint16_t length;
        receiveShortMessage(clientfd, &length, &flag);               // Receiving legth of the done list
        if (flag) {
            return 1;
        }
        printf("Receiving the message 06, and message's length, %d\n", length);
        uint32_t listEl;
        for (int i = 0; i < length; i ++) {
            receiveListElement(clientfd, &listEl, &flag);
            printf("Receiving the message 06, and message's content, %x\n", listEl);
        }
        printf("All done receiving\n");

        // 6. Close the client's socket.
        close(clientfd);

    }
    return 0;
}
