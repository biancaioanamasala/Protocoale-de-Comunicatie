#ifndef _REQUESTS_
#define _REQUESTS_

#include <string>

char *compute_get_request(char *host, char *url, std::string &jwt,
                          std::string &cookies);

char *compute_post_request(char *host, char *url, char *content_type, const std::string &body_data,
                           std::string &cookies, std::string &jwt);

char *compute_delete_request(char *host, char *url, char *content_type, std::string &jwt);

#endif
