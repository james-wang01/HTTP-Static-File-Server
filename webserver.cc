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
#include <fstream>

#include "HTTPRequestParser.h"

#define BACKLOG 10  /* pending connections queue size */

using namespace std;

int fileToBuffer(string file, char* &buffer) {
    ifstream f;
    int length;
    if (file.substr(file.find(".", 1) - 1).compare("txt") == 0 ||
        file.substr(file.find(".", 1) - 1).compare("html") == 0) {
            f.open(file);
    } else {
        f.open(file, ios::binary); 
    }
    if (f.fail()) {
        return -1;
    }
    f.seekg(0, ios::end);    
    length = f.tellg();           
    f.seekg(0, ios::beg);    
    buffer = new char[length];    
    f.read(buffer, length);       
    f.close();   
    return length;
}

string contentType(string url) {
    int pos = url.find(".");
    if (pos == string::npos) {
        return "application/octet-stream";
    } else {
        string s = url.substr(pos + 1);
        if (s.compare("txt") == 0) { return "text/plain"; }
        if (s.compare("html") == 0) { return "text/html"; }
        if (s.compare("jpg") == 0) { return "image/jpg"; }
        if (s.compare("png") == 0) { return "image/png"; }
        if (s.compare("gif") == 0) { return "image/gif"; }
    }
    return "application/octet-stream";
}

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

        // Parse Request
        HTTPRequest request;
        HTTPRequestParser parser;
        parser.Parse(new_fd, &request);
        unordered_map<string, string> map = request.ToMap();

        // for (auto& x : map) {
        //     cout << x.first << ": " << x.second << endl;
        // }

        printf("REQUEST COMPLETED\n");

        // TODO(!): Create Response
        char* buffer;
        int length = fileToBuffer("." + map["URL"], buffer);

        if (length == -1) {
            continue;
        }
        
        string response;
        response.append("HTTP/1.1 200 OK\r\n");
        response.append("Content-Length: " + to_string(length) + "\r\n");
        response.append("Content-Type: " + contentType(map["URL"]) +"\r\n");
        response.append("\r\n");
        response.append(buffer);

        // TODO(!): Send Response
        write(new_fd, response.c_str(), response.size() - 1);
        delete buffer;
        close(new_fd);
    }
}