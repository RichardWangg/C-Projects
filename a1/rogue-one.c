#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT "2520"
#define PLANS_FILE "deathstarplans.dat"
#define RESPONSE_SIZE 64

typedef struct {
    char * data;
    int length;
} buffer;

extern int errno;

/* This function loads the file of the Death Star
   plans so that they can be transmitted to the
   awaiting Rebel Fleet. It takes no arguments, but
   returns a buffer structure with the data. It is the
   responsibility of the caller to deallocate the 
   data element inside that structure.
   */ 
buffer load_plans( );

int main( int argc, char** argv ) {

    if ( argc != 2 ) {
        printf( "Usage: %s IP-Address\n", argv[0] );
        return -1;
    }
    printf("Planning to connect to %s.\n", argv[1]);

    struct addrinfo hints; 
    struct addrinfo *res; 
    int sockfd;
    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET; 
    hints.ai_socktype = SOCK_STREAM; 

    int result = getaddrinfo(argv[1], PORT, &hints, &res);
    if (result != 0) {
        return -1;
    }

    // Create a socket
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        freeaddrinfo(res);
        return -1;
    }

    // Connect to the server
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect");
        close(sockfd);
        freeaddrinfo(res);
        return -1;
    }

    buffer buf = load_plans();

    // Send the plans
    int total_sent = 0;
    while (total_sent < buf.length) {
        ssize_t bytes_sent = send(sockfd, buf.data + total_sent, buf.length - total_sent, 0);
        if (bytes_sent == -1) {
            perror("send");
            close(sockfd);
            free(buf.data);
            freeaddrinfo(res);
            return -1;
        }
        total_sent += bytes_sent;
    }

    // Receive response
    char response[RESPONSE_SIZE + 1]; // +1 for null terminator
    ssize_t bytes_received = recv(sockfd, response, RESPONSE_SIZE, 0);
    if (bytes_received == -1) {
        perror("recv");
        printf("%s", response);
        close(sockfd);
        free(buf.data);
        freeaddrinfo(res);
        return -1;
    } else if (bytes_received == 0) {
        printf("Connection closed by peer");
    } else {
        response[bytes_received] = '\0'; // Null-terminate the received data
        printf("%s", response);
    }

    close(sockfd);
    free(buf.data);
    freeaddrinfo(res);
    return 0;
}

buffer load_plans( ) {
    struct stat st;
    stat( PLANS_FILE, &st );
    ssize_t filesize = st.st_size;
    char* plansdata = malloc( filesize );
    int fd = open( PLANS_FILE, O_RDONLY );
    memset( plansdata, 0, filesize );
    read( fd, plansdata, filesize );
    close( fd );

    buffer buf;
    buf.data = plansdata;
    buf.length = filesize;

    return buf;
}
