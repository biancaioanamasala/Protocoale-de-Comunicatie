#include "helpers.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>


#include "buffer.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

using nlohmann::json;

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line) {
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag) {
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd) {
    close(sockfd);
}

void send_to_server(int sockfd, char *message) {
    int bytes, sent = 0;
    int total = strlen(message);

    do {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd) {
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;
    do {
        int bytes = read(sockfd, response, BUFLEN);
        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t)bytes);

        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;

            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH,
                                                               CONTENT_LENGTH_SIZE);

            if (content_length_start < 0) {
                continue;
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, nullptr, 10);
            break;
        }
    } while (true);
    size_t total = content_length + (size_t)header_end;

    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t)bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str) {
    return strstr(str, "{\"");
}

std::string server_request(int *sockfd, char *host, json data, char *address, std::string cookie, std::string jwt, int option) {
    char *message;
    switch (option) {
        case 0:
            message = compute_post_request((char *)"34.118.48.238", address,
                                           (char *)"application/json",
                                           data.dump(), cookie, jwt);
            break;
        case 1:
            message = compute_get_request((char *)"34.118.48.238", address,
                                          jwt, cookie);
            break;
        case 2:
            message = compute_delete_request((char *)"34.118.48.238", address,
                                            (char *)"application/json", jwt);
            break;
        case 3:
            message = compute_post_request((char *)"34.118.48.238", address,
                                            (char *)"application/json", 
                                            data.dump(), cookie, jwt);
            break;
    }
    // este deschisa conexiunea cu serverul
    *sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

    // mesajul este trimis catre server
    send_to_server(*sockfd, message);
    free(message);

    // primim raspuns de la server
    return receive_from_server(*sockfd);
}

int get_bookid(std::string id) {
    // object from the class stringstream
    std::stringstream convertString(id);

    // The object has the value 12345 and stream
    // it to the integer x
    int x = 0;
    convertString >> x;
    std::string str = std::to_string(x);
    if (str.compare(id) != 0 || x < 0) {
        return -1;
    }
    return x;
}