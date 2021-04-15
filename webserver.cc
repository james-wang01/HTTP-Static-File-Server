#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include "HTTPRequestParser.h"

#define BACKLOG 10  /* pending connections queue size */

using namespace std;

int main(int argc, char *argv[]) {
    int port;
    if (argc != 2) {
        perror("Usage: ./webserver <port>");
        exit(1);
    } else {
        port = atoi(argv[1]);
    }
    
    int sockfd, new_fd;            /* listen on sock_fd, new connection on new_fd */
    struct sockaddr_in my_addr;    /* my address */
    struct sockaddr_in their_addr; /* connector addr */
    socklen_t sin_size;

    /* create a socket */
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    /* set the address info */
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port); /* short, network byte order */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));

    /* bind the socket */
    if (bind(sockfd, (struct sockaddr *)&my_addr,
             sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    while (1) { 
        /* main accept() loop */
        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
        {
            perror("accept");
            continue;
        }
        
        HTTPRequest request;
        HTTPRequestParser parser;
        // Parse Request
        parser.Parse(new_fd, &request);
        // printf("%s", request.ToString().c_str());

        unordered_map<string, string> map = request.ToMap();

        for (auto& x : map) {
            cout << x.first << ": " << x.second << endl;
        }

        printf("REQUEST COMPLETED\n");
        // TODO(!): Create Response

        // TODO(!): Send Response

        close(new_fd);
    }
}