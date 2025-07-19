#ifndef APISERVER_H
#define APISERVER_H

#define APISERVER_VERSION "0.0.1"

#include <regex.h>

#define HTTP_METHOD_MAP(XX) \
    XX(0, DELETE, DELETE)   \
    XX(1, GET, GET)         \
    XX(2, HEAD, HEAD)       \
    XX(3, POST, POST)       \
    XX(4, PUT, PUT)

// HTTP_##name
enum http_method {
#define XX(num, name, string) HTTP_##name = num,
    HTTP_METHOD_MAP(XX)
#undef XX
        HTTP_CUSTOM_METHOD
};

typedef int (*api_handler)(char* url, int client, char* in, int inlen);

typedef struct api_route {
    enum http_method method;
    char* url;
    regex_t url_re;
    api_handler handler;
} api_route_t;

#ifdef __cplusplus
extern "C" {
#endif

int http_apiserver_init(int port, char* bindaddr);
int http_apiserver_register(enum http_method method, char* url, api_handler handler);
void http_apiserver_process_request(int fd);
int http_apiserver_send_response(int client, int code, char* desc, char* content, int len);
void bad_request(int);
// void cat(int, FILE*);
// void cannot_execute(int);
// void error_die(const char*);
// void execute_cgi(int, const char*, const char*, const char*);
int get_line(int, char*, int);
// void headers(int, const char*);
void not_found(int);
// void serve_file(int, const char*);
// int startup(u_short*);
void unimplemented(int);

#ifdef __cplusplus
}
#endif

#endif