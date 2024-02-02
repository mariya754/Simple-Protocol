#include <sys/socket.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
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
          printf("Error sending to the socket.");
          *flag = 1;
    } else if (*message > 0x06) {
            printf("Error: Message doesn't exist");
            *flag = 1;
        }
}
void receiveShortMessage(int sockfd, uint16_t *message, int* flag) {
    int brec = recv(sockfd, message, 2, 0);
        if (brec < 0) {
            printf("Error sending to the socket.");
            *flag = 1;
        }
    *message = ntohs(*message);
}
void sendListElement(int sockfd, uint32_t listEl, int* flag) {
    listEl = htonl(listEl);
    int bsent = send(sockfd, &listEl, 4, 0);
        if (bsent < 0) {
            printf("Error sending to the socket.");
            *flag = 1;
        }
}

int main(int argc, char** argv) {
    // 1. Read the server address and port from the command line.
    if ( argc < 3 ) {
        fprintf(stderr, "Usage: %s <server> <port>\n",argv[0]);
        return 1;
    }

    struct addrinfo hints;
    struct addrinfo* addrlist;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ( getaddrinfo(argv[1], argv[2], &hints, &addrlist) ) {
        fprintf(stderr, "Error looking up %s:%s\n",argv[1],argv[2]);
    return 1;
    }
    if ( NULL == addrlist ) {
        fprintf(stderr, "No address found for %s:%s\n",argv[1],argv[2]);
        return 1;
    }
    // 2. Create a socket.
    int sockfd;
    sockfd = socket(addrlist->ai_family,
    addrlist->ai_socktype,
    addrlist->ai_protocol);
    if ( sockfd < 0 ) {
        fprintf(stderr, "Socket creation failed with error %d\n", errno);
        return 1;
    }
    int e;
    // 3. Connect to the server.
    e = connect(sockfd, addrlist->ai_addr, addrlist->ai_addrlen);
    if ( e < 0 ) {
        fprintf(stderr, "Could not connect\n");
        close(sockfd);
        return 1;
    }
    // 4. Exchange data with the server.

    // creating a flag for errors 
    int flag = 0;

    // Send message 01 to connect to the server
    uint8_t message = 0x01;
    sendCharMessage(sockfd, message, &flag);
    if (flag) {
        return 1;
    }
    
    // Send contents of message 01 to the server
    uint16_t contents= 0x0124;
    sendShortMessage(sockfd, contents, &flag);
    if (flag) {
        return 1;
    }
    printf("Sending the message 01, and message's content, %x\n", message);

    // Receive message 02 from the server
    message = 0;
    receiveCharMessage(sockfd, &message, &flag);
    if (flag) {
        return 1;
    }
    // Receive the contents (client id) of message 02
    uint16_t u16_client_id;
    receiveShortMessage(sockfd, &u16_client_id, &flag);
    if (flag) {
        return 1;
    }
    printf("Receiving the message 02, and message's content, %d\n", message);

    //Send message 03 to request work from the server
    message = 0x03;
    sendCharMessage(sockfd, message, &flag);
    if (flag) {
        return 1;
    }
    // Send the contents of 03 (client id) to request work
    sendShortMessage(sockfd, u16_client_id, &flag);
    if (flag) {
        return 1;
    }
    printf("Sending the message 03, and message's content, %x\n", message);

    // Receive message 04
    receiveCharMessage(sockfd, &message, &flag);
    if (flag) {
        return 1;
    }
    // Receive contents of message 04 (work to do)
    uint32_t workToDo;
    int brec = recv(sockfd, &workToDo, 4, 0);
    if (brec < 0) {
        printf("Error receiving to the socket.");
        return 1;
    }
    printf("Receiving the message 04, and message's content, %d\n", workToDo);
    workToDo = ntohl(workToDo);

    // Send message 03 to request work AGAIN
    message = 0x03;
    sendCharMessage(sockfd, message, &flag);
    if (flag) {
        return 1;
    }
    // Send the contents of 03 (client id) to request work AGAIN
    sendShortMessage(sockfd, u16_client_id, &flag);
    if (flag) {
        return 1;
    }
    printf("Sending the message 03, and message's content, %x\n", message);

    // Receive message 05 about that work was sent results wasn't received yet
    receiveCharMessage(sockfd, &message, &flag);
    if (flag) {
        return 1;
    }
    printf("Receiving the message 05, and message's content is none\n");

    // Do the work!!!
    factor_list_t *factored_list = factor(workToDo);

    // Send message 06
    message = 0x06;
    sendCharMessage(sockfd, message, &flag);
    if (flag) {
        return 1;
    }
    // Send contents of message 06 (work done)
    sendShortMessage(sockfd, u16_client_id, &flag);        // Sending client id 
    if (flag) {
        return 1;
    }
    printf("Sending the message 06, and message's content, %x\n", u16_client_id);
    uint16_t length = factored_list->length;
    sendShortMessage(sockfd, length, &flag);               // Sending legth of the done list
    if (flag) {
        return 1;
    }
    // length = htons(length);
    printf("Sending the message 06, and message's content, %d\n", length);
    // Sending factored_list
    factor_link_t *el_ref = factored_list->head;      // reference to the head of the list
    for (int i = 0; i < length; i ++) {
        uint32_t element = el_ref->value;
        sendListElement(sockfd, element, &flag);
        printf("Sending the message 06, and message's content, %x\n", element);
        el_ref = el_ref->next;
    }
    printf("All done sending\n");
    delete_list(factored_list);

    // 5. Close the socket.
    
    close(sockfd);
    return 0;
}

