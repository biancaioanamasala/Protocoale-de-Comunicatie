#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include "nlohmann/json.hpp"
#include "requests.h"

#ifndef _HELPERS_
#define _HELPERS_

#define BUFLEN 4096
#define LINELEN 1000
using nlohmann::json;

void error(const char *msg);

void compute_message(char *message, const char *line);

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

void close_connection(int sockfd);

void send_to_server(int sockfd, char *message);

char *receive_from_server(int sockfd);

char *basic_extract_json_response(char *str);

std::string server_request(int *sockfd, char *host, json data, char *address, std::string cookie, std::string jwt, int option);

int get_bookid(std::string id);

#endif
