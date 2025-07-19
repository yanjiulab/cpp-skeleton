
#ifndef LIBSOCK_H
#define LIBSOCK_H

#define LIBSOCK_VERSION "0.0.1"

#include <netinet/in.h>
#include <sys/un.h> // for un
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
                            Socket Address Union
----------------------------------------------------------------------------*/
typedef union sockunion {
    struct sockaddr sa;
    struct sockaddr_in sin;
    struct sockaddr_in6 sin6;
    struct sockaddr_un sun;
} sockaddr_t;

enum connect_result { connect_error,
                      connect_success,
                      connect_in_progress };

/* Interface index type. */
typedef signed int ifindex_t;

/* Default address family. */
#define AF_INET_UNION      AF_INET6

#define SOCKADDR_STRLEN    64 // ipv4:port | [ipv6]:port

#define sockaddr_family(X) (X)->sa.sa_family
#define sockaddr_ip(X)     (X)->sin.sin_addr.s_addr

/* Macro to set link local index to the IPv6 address.  For KAME IPv6 stack. */
#ifdef KAME
#define IN6_LINKLOCAL_IFINDEX(a) ((a).s6_addr[2] << 8 | (a).s6_addr[3])
#define SET_IN6_LINKLOCAL_IFINDEX(a, i)     \
    do {                                    \
        (a).s6_addr[2] = ((i) >> 8) & 0xff; \
        (a).s6_addr[3] = (i) & 0xff;        \
    } while (0)
#else
#define IN6_LINKLOCAL_IFINDEX(a)
#define SET_IN6_LINKLOCAL_IFINDEX(a, i)
#endif /* KAME */

// API
void sockaddr_init(sockaddr_t*);
sockaddr_t* sockaddr_new(const char* str);
sockaddr_t* sockaddr_dup(const sockaddr_t*); // Duplicate sockaddr
void sockaddr_free(sockaddr_t*);
// Compare ipv6 address
int in6addr_cmp(const struct in6_addr* addr1, const struct in6_addr* addr2);
// Only compare ip address
int sockaddr_cmp(const sockaddr_t*, const sockaddr_t*);
// Only compare ip address
bool sockaddr_same(const sockaddr_t*, const sockaddr_t*);
int sockaddr_is_null(const sockaddr_t* su);
// sockunioin -> ip string
const char* inet_sutop(const sockaddr_t* su, char* str);
// ip string -> sockunioin
int inet_ptosu(const char*, sockaddr_t*);
// `ip;port` format string -> sockunioin
int str2sockaddr(const char*, sockaddr_t*);
// sockunioin -> `ipv4:port | [ipv6]:port` format string
const char* sockaddr2str(const sockaddr_t*, char*, size_t);
// Same as sockaddr2str, but return malloced string format
char* sockaddr_str(const sockaddr_t*);
// Print sockaddr structure
void sockaddr_print(const sockaddr_t*);

// Get address in network order
const uint8_t* sockaddr_get_addr(const sockaddr_t*);
size_t family2addrsize(int family);
// AF_INET: 4 (32 bit); AF_INET6: 16 (128 bit)
size_t sockaddr_get_addrlen(const sockaddr_t*);
// Return sizeof sockaddr_t.
int sockaddr_sizeof(const sockaddr_t* su);
// Set address with bytes format
void sockaddr_set(sockaddr_t*, int family, const uint8_t* addr, size_t bytes);
// Set IP address with string format, only ip, not dns.
int sockaddr_set_ip(sockaddr_t* addr, const char* host);
void sockaddr_set_port(sockaddr_t* addr, int port);
int sockaddr_set_ipport(sockaddr_t* addr, const char* host, int port);

unsigned int sockaddr_hash(const sockaddr_t*);
int sockaddr_socket(const sockaddr_t* su);
int sockaddr_accept(int sock, sockaddr_t*);
int sockaddr_stream_socket(sockaddr_t*);
int sockaddr_bind(int sock, sockaddr_t*, unsigned short, sockaddr_t*);
enum connect_result sockaddr_connect(int fd, const sockaddr_t* su, unsigned short port, ifindex_t);
sockaddr_t* sockaddr_getsockname(int);
sockaddr_t* sockaddr_getpeername(int);

#ifdef __cplusplus
}
#endif
#endif
