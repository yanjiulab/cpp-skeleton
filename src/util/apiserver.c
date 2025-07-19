#include "apiserver.h"
#include "libanet.h"
#include "libvector.h"
#include "libstr.h"

#include <sys/socket.h>
#include "cJSON.h"

#define SERVER_STRING "Server: ca-http-apiserver/0.1.0\r\n"
#define ISspace(x)    isspace((int)(x))

#define STDIN         0
#define STDOUT        1
#define STDERR        2

/*---------------------------------------------------------------------------
                            HTTP
----------------------------------------------------------------------------*/
const char* http_method_str(enum http_method method) {
    switch (method) {
#define XX(num, name, string) \
    case HTTP_##name: return #string;
        HTTP_METHOD_MAP(XX)
#undef XX
    default: return "<unknown>";
    }
}

/*---------------------------------------------------------------------------
                            RESTful API
----------------------------------------------------------------------------*/
vector_t api_routes = NULL;

int http_apiserver_init(int port, char* bindaddr) {
    int fd = net_tcp_server(NULL, port, bindaddr, 0);
    assert(fd != ANET_ERR);

    api_routes = vector_new(8);

    return fd;
}

int http_apiserver_deinit() {
    // todo: delete all and free items
    vector_clear(api_routes);
    vector_free(api_routes);
    api_routes = NULL;
}

/**
 * @brief register url to http api server
 * @param url regex pattern. Use "^str$" for re exact string match.
 * @param handler called if both method and url are matched
 * @return 
 */
int http_apiserver_register(enum http_method method, char* url, api_handler handler) {
    api_route_t* api = calloc(1, sizeof(*api));
    api->method = method;
    api->url = strdup(url);
    api->handler = handler;

    int ret = regcomp(&api->url_re, api->url, REG_EXTENDED);
    if (ret != 0) {
        printe("re compile error");
        return -1;
    }
    if (!api_routes) api_routes = vector_new(8);

    vector_push(api_routes, api);

    return 0;
}

static int http_apiserver_dispatch(int client, enum http_method method,
                                   char* url, char* content, int content_length) {

    printd("dispatch %6s %s, Content-Length: %d", http_method_str(method), url, content_length);

    int ret;
    regmatch_t matches[2];
    api_route_t* api;
    int i;
    vector_foreach(api_routes, api, i) {
        if (method != api->method) continue;

        /* re match */
        ret = regexec(&api->url_re, url, 0, NULL, 0);
        if (ret == 0) {
            return api->handler(url, client, content, content_length);
        }

        /* exact string match */
        // if (strcasecmp(url, api->url) == 0) {
        //     return api->handler(url, client, content, content_length);
        // }
    }

    return -1;
}

/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return.  Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
void http_apiserver_process_request(int fd) {

    int client = fd;
    char buf[1024];
    size_t numchars;
    char method[255];
    char url[255];
    char path[512];
    size_t i, j;
    struct stat st;
    int cgi = 0;
    char* query_string = NULL;

    /* Parse request line */
    numchars = get_line(client, buf, sizeof(buf)); // this will remove \r
    // printd("request line: %s", buf);

    i = 0;
    j = 0;
    while (!ISspace(buf[i]) && (i < sizeof(method) - 1)) {
        method[i] = buf[i];
        i++;
    }
    j = i;
    method[i] = '\0';

    if (strcasecmp(method, "GET") &&
        strcasecmp(method, "POST") &&
        strcasecmp(method, "PUT") &&
        strcasecmp(method, "PATCH") &&
        strcasecmp(method, "DELETE")) {
        unimplemented(client);
        return;
    }

    i = 0;
    while (ISspace(buf[j]) && (j < numchars))
        j++;
    while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < numchars)) {
        url[i] = buf[j];
        i++;
        j++;
    }
    url[i] = '\0';

    /* Process by method */
    int content_length = -1;
    numchars = 1;
    buf[0] = 'A';
    buf[1] = '\0';
    int ret = -1;
    if (strcasecmp(method, "GET") == 0) {
        /* read & discard headers */
        while ((numchars > 0) && strcmp("\n", buf))
            numchars = get_line(client, buf, sizeof(buf));
        /* dispatch */
        ret = http_apiserver_dispatch(fd, HTTP_GET, url, NULL, 0);
        if (ret < 0) {
            not_found(client);
            return;
        }
    } else if (strcasecmp(method, "POST") == 0) {
        /* read header and get Content-Length */
        numchars = get_line(client, buf, sizeof(buf));
        while ((numchars > 0) && strcmp("\n", buf)) {
            buf[15] = '\0';
            if (strcasecmp(buf, "Content-Length:") == 0) {
                content_length = atoi(&(buf[16]));
            }
            numchars = get_line(client, buf, sizeof(buf));
        }
        if (content_length == -1) {
            bad_request(client);
            return;
        }
        /* read content */
        char* content = calloc(1, content_length);
        recv(fd, content, content_length, 0);
        // printd("%s", content);
        // DUMP_BUFFER(content, content_length);

        /* dispatch */
        ret = http_apiserver_dispatch(fd, HTTP_POST, url, content, content_length);
        if (ret < 0) {
            not_found(client);
            return;
        }
    } else if (strcasecmp(method, "DELETE") == 0) {
        /* read & discard headers */
        while ((numchars > 0) && strcmp("\n", buf))
            numchars = get_line(client, buf, sizeof(buf));
        /* dispatch */
        ret = http_apiserver_dispatch(fd, HTTP_DELETE, url, NULL, 0);
        if (ret < 0) {
            not_found(client);
            return;
        }
    } else {
        /* HEAD or others */
        unimplemented(client);
        return;
    }

    // not handle
    if (ret < 0) {
        not_found(client);
    }
}

int http_apiserver_send_response(int client, int code, char* desc, char* content, int len) {
    char buf[1024];

    sprintf(buf, "HTTP/1.0 %d %s\r\n", code, desc);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    // send(client, buf, strlen(buf), 0);
    // sprintf(buf, "Connection: keep-alive\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: application/json; charset=UTF-8\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: %d\r\n", len);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    if (len > 0) {
        send(client, content, len, 0);
    }
}

/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/
int get_line(int sock, char* buf, int size) {
    int i = 0;
    char c = '\0';
    int n;

    while ((i < size - 1) && (c != '\n')) {
        n = recv(sock, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        if (n > 0) {
            if (c == '\r') {
                n = recv(sock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        } else
            c = '\n';
    }
    buf[i] = '\0';

    return (i);
}

/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 * Parameters: client socket */
/**********************************************************************/
void bad_request(int client) {
    printd("HTTP/1.0 400 BAD REQUEST");
    char buf[1024];
    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: 108\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><HEAD><TITLE>Bad Request\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>Your browser sent a bad request.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Inform the client that the requested web method has not been
 * implemented.
 * Parameter: the client socket */
/**********************************************************************/
void unimplemented(int client) {
    printd("HTTP/1.0 501 Method Not Implemented");
    char buf[1024];
    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: 121\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void not_found_html(int client) {
    printd("HTTP/1.0 404 NOT FOUND");
    char buf[1024];
    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: 164\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "your request because the resource specified\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "is unavailable or nonexistent.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

void not_found(int client) {
    char buf[1024];
    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: application/json; charset=UTF-8\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: %d\r\n", 113);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf,
            "{\"msg\":\"The server could not fulfill "
            "your request because the resource specified "
            "is unavailable or nonexistent.\"}");
    send(client, buf, strlen(buf), 0);
}