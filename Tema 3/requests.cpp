#include "requests.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "helpers.h"

char *compute_post_request(char *host, char *url, char *content_type, const std::string &body_data,
                           std::string &cookies, std::string &jwt) {
    // se aloca bufferul pentru mesaj
    char *message = (char *)(calloc(BUFLEN, sizeof(char)));
    if (message == NULL) {
        return NULL;
    }
    // se aloca bufferul pentru linii
    char *line = (char *)(calloc(LINELEN, sizeof(char)));
    if (line == NULL) {
        return NULL;
    }
    int content_len = 0;

    content_len = body_data.length();

    // se scrie tipul cererii
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // se adauga host-ul
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // se adauga cookies daca exista
    if (!cookies.empty()) {
        sprintf(line, "Cookies: %s", cookies.c_str());
        compute_message(message, line);
    }

    // se adauga tipul mesajului
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    // daca exista JWT, este adaugat
    if (!jwt.empty()) {
        sprintf(line, "%s", jwt.c_str());
        compute_message(message, line);
    }
    
    // se adauga lungimea campului de date
    sprintf(line, "Content-Length: %d", content_len);
    compute_message(message, line);

    compute_message(message, "");

    memset(line, 0, LINELEN);
    if (!body_data.empty()) {
        compute_message(message, body_data.c_str());
    }
    free(line);
    return message;
}

char *compute_get_request(char *host, char *url, std::string &jwt,
                          std::string &cookies) {
    // se aloca bufferele ce stocheaza mesajula
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    if (message == NULL) {
        return NULL;
    }
    // se aloca bufferul pentru linii
    char *line = (char *)calloc(LINELEN, sizeof(char));
    if (line == NULL) {
        return NULL;
    }

    // se scrie tipul cererii
    sprintf(line, "GET %s HTTP/1.1", url);
    compute_message(message, line);

    // se adauga ip-ul hostului
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // daca exista token-ul JWT, atunci este adaugat
    if (!jwt.empty()) {
        sprintf(line, "%s", jwt.c_str());
        compute_message(message, line);
    }

    // daca exista cookies, vor fi adaugate
    if (!cookies.empty()) {
        sprintf(line, "Cookie: %s", cookies.c_str());
        compute_message(message, line);
    }

    compute_message(message, "");
    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char *content_type,
                             std::string &jwt) {
    // se aloca memorie pentru mesaj
    char *message = (char *)(calloc(BUFLEN, sizeof(char)));
    if (message == NULL) {
        return NULL;
    }
    // se aloca memorie pentru linii
    char *line = (char *)(calloc(LINELEN, sizeof(char)));
    if (line == NULL) {
        return NULL;
    }

    // in functie de tipul request-ului, se alege DELETE sau POST
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // se adauga host-ul
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // se adauga tipul continulului
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    // daca exista JWT, este adaugat
    if (!jwt.empty()) {
        sprintf(line, "%s", jwt.c_str());
        compute_message(message, line);
    }
    compute_message(message, "");

    memset(line, 0, LINELEN);
    free(line);
    return message;
}