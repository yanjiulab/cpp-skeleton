
#include "libsock.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>

#ifndef printe
#define printe(fmt, ...)                                                           \
    do {                                                                           \
        fprintf(stderr, "[%s:%d] " fmt "\r\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        fflush(stderr);                                                            \
    } while (0)
#endif

void sockaddr_init(sockaddr_t* su) {
    memset(su, 0, sizeof(sockaddr_t));
}

sockaddr_t* sockaddr_new(const char* str) {
    sockaddr_t* su = calloc(1, sizeof(sockaddr_t));

    if (!str2sockaddr(str, su))
        return su;

    free(su);
    return NULL;
}

sockaddr_t* sockaddr_dup(const sockaddr_t* su) {
    sockaddr_t* dup = calloc(1, sizeof(sockaddr_t));
    if (su)
        memcpy(dup, su, sizeof(sockaddr_t));
    return dup;
}

void sockaddr_free(sockaddr_t* su) {
    free(su);
}

/* If same family and same prefix return 1. */
bool sockaddr_same(const sockaddr_t* su1, const sockaddr_t* su2) {
    bool ret = false;

    if (su1->sa.sa_family != su2->sa.sa_family)
        return false;

    switch (su1->sa.sa_family) {
    case AF_INET:
        ret = memcmp(&su1->sin.sin_addr, &su2->sin.sin_addr,
                     sizeof(struct in_addr));
        break;
    case AF_INET6:
        ret = memcmp(&su1->sin6.sin6_addr, &su2->sin6.sin6_addr,
                     sizeof(struct in6_addr));
        if ((ret == false) && IN6_IS_ADDR_LINKLOCAL(&su1->sin6.sin6_addr)) {
            /* compare interface indices */
            if (su1->sin6.sin6_scope_id && su2->sin6.sin6_scope_id)
                ret = (su1->sin6.sin6_scope_id == su2->sin6.sin6_scope_id)
                          ? false
                          : true;
        }
        break;
    }
    if (ret == 0)
        return true;
    else
        return false;
}

int in6addr_cmp(const struct in6_addr* addr1, const struct in6_addr* addr2) {
    unsigned int i;
    const uint8_t *p1, *p2;

    p1 = (const uint8_t*)addr1;
    p2 = (const uint8_t*)addr2;

    for (i = 0; i < sizeof(struct in6_addr); i++) {
        if (p1[i] > p2[i])
            return 1;
        else if (p1[i] < p2[i])
            return -1;
    }
    return 0;
}

int sockaddr_cmp(const sockaddr_t* su1, const sockaddr_t* su2) {
    if (su1->sa.sa_family > su2->sa.sa_family)
        return 1;
    if (su1->sa.sa_family < su2->sa.sa_family)
        return -1;

    if (su1->sa.sa_family == AF_INET) {
        if (ntohl(sockaddr_ip(su1)) == ntohl(sockaddr_ip(su2)))
            return 0;
        if (ntohl(sockaddr_ip(su1)) > ntohl(sockaddr_ip(su2)))
            return 1;
        else
            return -1;
    }
    if (su1->sa.sa_family == AF_INET6)
        return in6addr_cmp(&su1->sin6.sin6_addr, &su2->sin6.sin6_addr);
    return 0;
}

int sockaddr_is_null(const sockaddr_t* su) {
    unsigned char null_s6_addr[16] = {0};

    switch (sockaddr_family(su)) {
    case AF_UNSPEC:
        return 1;
    case AF_INET:
        return (su->sin.sin_addr.s_addr == 0);
    case AF_INET6:
        return !memcmp(su->sin6.sin6_addr.s6_addr, null_s6_addr,
                       sizeof(null_s6_addr));
    default:
        return 0;
    }
}

const char* inet_sutop(const sockaddr_t* su, char* str) {
    switch (su->sa.sa_family) {
    case AF_INET:
        inet_ntop(AF_INET, &su->sin.sin_addr, str, INET_ADDRSTRLEN);
        break;
    case AF_INET6:
        inet_ntop(AF_INET6, &su->sin6.sin6_addr, str, INET6_ADDRSTRLEN);
        break;
    }
    return str;
}

int inet_ptosu(const char* str, sockaddr_t* su) {
    int ret;

    if (str == NULL)
        return -1;

    memset(su, 0, sizeof(sockaddr_t));

    ret = inet_pton(AF_INET, str, &su->sin.sin_addr);
    if (ret > 0) /* Valid IPv4 address format. */
    {
        su->sin.sin_family = AF_INET;
#ifdef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
        su->sin.sin_len = sizeof(struct sockaddr_in);
#endif /* HAVE_STRUCT_SOCKADDR_IN_SIN_LEN */
        return 0;
    }
    ret = inet_pton(AF_INET6, str, &su->sin6.sin6_addr);
    if (ret > 0) /* Valid IPv6 address format. */
    {
        su->sin6.sin6_family = AF_INET6;
#ifdef SIN6_LEN
        su->sin6.sin6_len = sizeof(struct sockaddr_in6);
#endif /* SIN6_LEN */
        return 0;
    }
    return -1;
}

int str2sockaddr(const char* s, sockaddr_t* su) {
    int ret;

    if (s == NULL)
        return -1;

    char* str = strdup(s);
    memset(su, 0, sizeof(sockaddr_t));

    // parse port
    char* pos = strrchr(str, ';');
    if (pos) {
        unsigned long port = strtoul(pos + 1, NULL, 10);
        su->sin.sin_port = port > 65535 ? 0 : htons(port);
        *pos = '\0';
    }

    // parse ip
    ret = inet_pton(AF_INET, str, &su->sin.sin_addr);
    if (ret > 0) /* Valid IPv4 address format. */
    {
        su->sin.sin_family = AF_INET;
#ifdef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
        su->sin.sin_len = sizeof(struct sockaddr_in);
#endif /* HAVE_STRUCT_SOCKADDR_IN_SIN_LEN */
        return 0;
    }
    ret = inet_pton(AF_INET6, str, &su->sin6.sin6_addr);
    if (ret > 0) /* Valid IPv6 address format. */
    {
        su->sin6.sin6_family = AF_INET6;
#ifdef SIN6_LEN
        su->sin6.sin6_len = sizeof(struct sockaddr_in6);
#endif /* SIN6_LEN */
        return 0;
    }
    return -1;
}

const char* sockaddr2str(const sockaddr_t* su, char* buf, size_t len) {

    char ip[SOCKADDR_STRLEN] = {0};
    switch (sockaddr_family(su)) {
    case AF_UNSPEC:
        snprintf(buf, len, "(unspec)");
        break;
    case AF_INET:
        inet_ntop(AF_INET, &su->sin.sin_addr, ip, INET_ADDRSTRLEN);
        snprintf(buf, len, "%s:%d", ip, ntohs(su->sin.sin_port));
        break;
    case AF_INET6:
        inet_ntop(AF_INET6, &su->sin6.sin6_addr, ip, INET6_ADDRSTRLEN);
        snprintf(buf, len, "[%s]:%d", ip, ntohs(su->sin6.sin6_port));
        break;
    default:
        snprintf(buf, len, "(af %d)", sockaddr_family(su));
        break;
    }
    return buf;
}

char* sockaddr_str(const sockaddr_t* su) {
    char* buf = calloc(1, SOCKADDR_STRLEN);
    sockaddr2str(su, buf, SOCKADDR_STRLEN);
    return buf;
}

void sockaddr_print(const sockaddr_t* su) {
    char buf[SOCKADDR_STRLEN] = {0};
    sockaddr2str(su, buf, SOCKADDR_STRLEN);
    puts(buf);
}

size_t family2addrsize(int family) {
    switch (family) {
    case AF_INET:
        return sizeof(struct in_addr);
    case AF_INET6:
        return sizeof(struct in6_addr);
    }
    return 0;
}

size_t sockaddr_get_addrlen(const sockaddr_t* su) {
    return family2addrsize(sockaddr_family(su));
}

const uint8_t* sockaddr_get_addr(const sockaddr_t* su) {
    switch (sockaddr_family(su)) {
    case AF_INET:
        return (const uint8_t*)&su->sin.sin_addr.s_addr;
    case AF_INET6:
        return (const uint8_t*)&su->sin6.sin6_addr;
    }
    return NULL;
}

int sockaddr_sizeof(const sockaddr_t* su) {
    int ret;

    ret = 0;
    switch (su->sa.sa_family) {
    case AF_INET:
        ret = sizeof(struct sockaddr_in);
        break;
    case AF_INET6:
        ret = sizeof(struct sockaddr_in6);
        break;
    }
    return ret;
}

void sockaddr_set(sockaddr_t* su, int family, const uint8_t* addr, size_t bytes) {
    if (family2addrsize(family) != bytes)
        return;

    sockaddr_family(su) = family;
    switch (family) {
    case AF_INET:
        memcpy(&su->sin.sin_addr.s_addr, addr, bytes);
        break;
    case AF_INET6:
        memcpy(&su->sin6.sin6_addr, addr, bytes);
        break;
    }
}

int sockaddr_set_ip(sockaddr_t* addr, const char* host) {
    if (!host || *host == '\0') {
        addr->sin.sin_family = AF_INET;
        addr->sin.sin_addr.s_addr = htonl(INADDR_ANY);
        return 0;
    }

    return str2sockaddr(host, addr); // only ip, not dns
    // return ResolveAddr(host, addr);
}

void sockaddr_set_port(sockaddr_t* addr, int port) {
    if (addr->sa.sa_family == AF_INET) {
        addr->sin.sin_port = htons(port);
    } else if (addr->sa.sa_family == AF_INET6) {
        addr->sin6.sin6_port = htons(port);
    }
}

int sockaddr_set_ipport(sockaddr_t* addr, const char* host, int port) {
    int ret = sockaddr_set_ip(addr, host);
    if (ret != 0)
        return ret;
    sockaddr_set_port(addr, port);
    // SOCKADDR_PRINT(addr);
    return 0;
}

/* Convert IPv4 compatible IPv6 address to IPv4 address. */
static void sockaddr_normalise_mapped(sockaddr_t* su) {
    struct sockaddr_in sin;

    if (su->sa.sa_family == AF_INET6 && IN6_IS_ADDR_V4MAPPED(&su->sin6.sin6_addr)) {
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = su->sin6.sin6_port;
        memcpy(&sin.sin_addr, ((char*)&su->sin6.sin6_addr) + 12, 4);
        memcpy(su, &sin, sizeof(struct sockaddr_in));
    }
}

/* return sockaddr_t structure : this function should be revised. */
static const char* sockaddr_log(const sockaddr_t* su, char* buf, size_t len) {
    switch (su->sa.sa_family) {
    case AF_INET:
        return inet_ntop(AF_INET, &su->sin.sin_addr, buf, len);

    case AF_INET6:
        return inet_ntop(AF_INET6, &(su->sin6.sin6_addr), buf, len);

    default:
        snprintf(buf, len, "af_unknown %d ", su->sa.sa_family);
        return buf;
    }
}

/* Return socket of sockaddr. */
int sockaddr_socket(const sockaddr_t* su) {
    int sock;

    sock = socket(su->sa.sa_family, SOCK_STREAM, 0);
    if (sock < 0) {
        char buf[SOCKADDR_STRLEN];
        printe("Can't make socket for %s : %s",
               sockaddr_log(su, buf, SOCKADDR_STRLEN),
               strerror(errno));
        return -1;
    }

    return sock;
}

/* Return accepted new socket file descriptor. */
int sockaddr_accept(int sock, sockaddr_t* su) {
    socklen_t len;
    int client_sock;

    len = sizeof(sockaddr_t);
    client_sock = accept(sock, (struct sockaddr*)su, &len);

    sockaddr_normalise_mapped(su);
    return client_sock;
}

/* Performs a non-blocking connect().  */
enum connect_result sockaddr_connect(int fd, const sockaddr_t* peersu,
                                     unsigned short port, ifindex_t ifindex) {
    int ret;
    sockaddr_t su;

    memcpy(&su, peersu, sizeof(sockaddr_t));

    switch (su.sa.sa_family) {
    case AF_INET:
        su.sin.sin_port = port;
        break;
    case AF_INET6:
        su.sin6.sin6_port = port;
#ifdef KAME
        if (IN6_IS_ADDR_LINKLOCAL(&su.sin6.sin6_addr) && ifindex) {
            su.sin6.sin6_scope_id = ifindex;
            SET_IN6_LINKLOCAL_IFINDEX(su.sin6.sin6_addr, ifindex);
        }
#endif /* KAME */
        break;
    }

    /* Call connect function. */
    ret = connect(fd, (struct sockaddr*)&su, sockaddr_sizeof(&su));

    /* Immediate success */
    if (ret == 0)
        return connect_success;

    /* If connect is in progress then return 1 else it's real error. */
    if (ret < 0) {
        if (errno != EINPROGRESS) {
            char str[SOCKADDR_STRLEN];
            printe("can't connect to %s fd %d : %s",
                   sockaddr_log(&su, str, sizeof(str)), fd,
                   strerror(errno));
            return connect_error;
        }
    }

    return connect_in_progress;
}

/* Make socket from sockaddr union. */
int sockaddr_stream_socket(sockaddr_t* su) {
    int sock;

    if (su->sa.sa_family == 0)
        su->sa.sa_family = AF_INET_UNION;

    sock = socket(su->sa.sa_family, SOCK_STREAM, 0);

    if (sock < 0)
        printe("can't make socket sockaddr_stream_socket");

    return sock;
}

/* Bind socket to specified address. */
int sockaddr_bind(int sock, sockaddr_t* su, unsigned short port,
                  sockaddr_t* su_addr) {
    int size = 0;
    int ret;

    if (su->sa.sa_family == AF_INET) {
        size = sizeof(struct sockaddr_in);
        su->sin.sin_port = htons(port);
#ifdef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
        su->sin.sin_len = size;
#endif /* HAVE_STRUCT_SOCKADDR_IN_SIN_LEN */
        if (su_addr == NULL)
            sockaddr_ip(su) = htonl(INADDR_ANY);
    } else if (su->sa.sa_family == AF_INET6) {
        size = sizeof(struct sockaddr_in6);
        su->sin6.sin6_port = htons(port);
#ifdef SIN6_LEN
        su->sin6.sin6_len = size;
#endif /* SIN6_LEN */
        if (su_addr == NULL) {
#ifdef LINUX_IPV6
            memset(&su->sin6.sin6_addr, 0, sizeof(struct in6_addr));
#else
            su->sin6.sin6_addr = in6addr_any;
#endif /* LINUX_IPV6 */
        }
    }

    ret = bind(sock, (struct sockaddr*)su, size);
    if (ret < 0) {
        char buf[SOCKADDR_STRLEN];
        printe("can't bind socket for %s : %s",
               sockaddr_log(su, buf, SOCKADDR_STRLEN),
               strerror(errno));
    }

    return ret;
}

unsigned int sockaddr_hash(const sockaddr_t* su) {
    // switch (sockaddr_family(su)) {
    // case AF_INET:
    //     return jhash_1word(su->sin.sin_addr.s_addr, 0);
    // case AF_INET6:
    //     return jhash2(su->sin6.sin6_addr.s6_addr32, array_size(su->sin6.sin6_addr.s6_addr32), 0);
    // }
    return 0;
}

/* After TCP connection is established.  Get local address and port. */
sockaddr_t* sockaddr_getsockname(int fd) {
    int ret;
    socklen_t len;
    union {
        struct sockaddr sa;
        struct sockaddr_in sin;
        struct sockaddr_in6 sin6;
        char tmp_buffer[128];
    } name;
    sockaddr_t* su;

    memset(&name, 0, sizeof(name));
    len = sizeof(name);

    ret = getsockname(fd, (struct sockaddr*)&name, &len);
    if (ret < 0) {
        printe("Can't get local address and port by getsockname: %s",
               strerror(errno));
        return NULL;
    }

    if (name.sa.sa_family == AF_INET) {
        su = calloc(1, sizeof(sockaddr_t));
        memcpy(su, &name, sizeof(struct sockaddr_in));
        return su;
    }
    if (name.sa.sa_family == AF_INET6) {
        su = calloc(1, sizeof(sockaddr_t));
        memcpy(su, &name, sizeof(struct sockaddr_in6));
        sockaddr_normalise_mapped(su);
        return su;
    }

    printe("Unexpected AFI received(%d) for sockaddr_getsockname call for fd: %d",
           name.sa.sa_family, fd);
    return NULL;
}

/* After TCP connection is established.  Get remote address and port. */
sockaddr_t* sockaddr_getpeername(int fd) {
    int ret;
    socklen_t len;
    union {
        struct sockaddr sa;
        struct sockaddr_in sin;
        struct sockaddr_in6 sin6;
        char tmp_buffer[128];
    } name;
    sockaddr_t* su;

    memset(&name, 0, sizeof(name));
    len = sizeof(name);
    ret = getpeername(fd, (struct sockaddr*)&name, &len);
    if (ret < 0) {
        printe("Can't get remote address and port: %s",
               strerror(errno));
        return NULL;
    }

    if (name.sa.sa_family == AF_INET) {
        su = calloc(1, sizeof(sockaddr_t));
        memcpy(su, &name, sizeof(struct sockaddr_in));
        return su;
    }
    if (name.sa.sa_family == AF_INET6) {
        su = calloc(1, sizeof(sockaddr_t));
        memcpy(su, &name, sizeof(struct sockaddr_in6));
        sockaddr_normalise_mapped(su);
        return su;
    }

    printe("Unexpected AFI received(%d) for sockaddr_getpeername call for fd: %d",
           name.sa.sa_family, fd);
    return NULL;
}