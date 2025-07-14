
#ifndef LIBANET_H
#define LIBANET_H

#define LIBANET_VERSION "0.0.1"

#include <libposix.h>

#define ANET_OK      0
#define ANET_ERR     -1

#define ANET_ERR_LEN 256

/* Flags used with certain functions. */
#define ANET_NONE    0
#define ANET_IP_ONLY (1 << 0)

#define SOCKADDR_STRLEN    64 // ipv4:port | [ipv6]:port

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
                            Socket Options
----------------------------------------------------------------------------*/
#ifdef OS_LINUX
#include <linux/filter.h>
#include <net/if.h>           // for struct ifreq
#include <sys/un.h>           // for un
#include <netpacket/packet.h> // for struct sockaddr_ll
#define SO_ATTACH_FILTER 26
#endif // OS_LINUX

int net_nonblock(char* err, int fd);
int net_block(char* err, int fd);
/* Enable the FD_CLOEXEC on the given fd to avoid fd leaks.
 * This function should be invoked for fd's on specific places
 * where fork + execve system calls are called. */
int net_cloexec(int fd);
/* Set TCP keep alive option to detect dead peers. The interval option
 * is only used for Linux as we are using Linux-specific APIs to set
 * the probe send time, interval, and count. */
int net_keepalive(char* err, int fd, int interval);
int net_enable_tcp_nodelay(char* err, int fd);
int net_disable_tcp_nodelay(char* err, int fd);
int net_enable_tcp_nopush(char* err, int fd);
int net_disable_tcp_nopush(char* err, int fd);
/* Set the socket send timeout (SO_SNDTIMEO socket option) to the specified
 * number of milliseconds, or disable it if the 'ms' argument is zero. */
int net_send_timeout(char* err, int fd, long long ms);
/* Set the socket receive timeout (SO_RCVTIMEO socket option) to the specified
 * number of milliseconds, or disable it if the 'ms' argument is zero. */
int net_recv_timeout(char* err, int fd, long long ms);
// 1. 0.0.0.0:21, 192.168.1.1:21 => failed if SO_REUSEADDR not set
// 2. reconnect on server TIME_WAIT => failed if SO_REUSEADDR not set
int net_set_reuse_addr(char* err, int fd);
// SO_REUSEPORT allow multiple sockets to bind same port in load balance mode (linux >= 3.9)
int net_set_reuse_port(char* err, int fd);
int net_attach_filter(char* err, int fd, struct sock_fprog fprog);
int net_enable_udp_broadcast(char* err, int fd);
int net_disable_udp_broadcast(char* err, int fd);
int net_bindtodev(char* err, int fd, char* ifname);
// Set the Type-Of-Service (TOS) field that is sent with every IP packet originating from this socket.
int net_set_send_tos(char* err, int sockfd, int tos);
// Return the Type-Of-Service (TOS) field that is sent with every IP packet originating from this socket.
int net_get_send_tos(char* err, int sockfd);
// Set the current time-to-live field that is used in every packet sent from this socket.
int net_set_send_ttl(char* err, int family, int sock, int ttl);
// Return the current time-to-live field that is used in every packet sent from this socket.
int net_get_send_ttl(char* err, int sockfd);
// Set the minimum TTL permited in TCP packet recieve from this socket.
int so_set_minttl(char* err, int family, int sock, int minttl);
// Set socket receive buffer size. see: /proc/sys/net/core/rmem_default
int net_recvbuf(int fd, int bufsize);
// Set socket send buffer size. see: /proc/sys/net/core/wmem_default
int net_sendbuf(int fd, int bufsize);
// When enabled, a close(2) or shutdown(2) will not return
// until all queued messages for the socket have been
// successfully sent or the linger timeout has been reached.
// Otherwise, the call returns immediately and the closing is
// done in the background.
int net_linger(int sockfd, int timeout);
// Don't send via a gateway, send only to directly connected hosts.
// The same effect can be achieved by setting the MSG_DONTROUTE flag
// on a socket send(2) operation. Expects an integer boolean flag.
int net_noroute(int sockfd, int on);
/**
 * When this flag is set, pass a IP_TTL control message with
 * the time-to-live field of the received packet as a 32 bit integer.
 * Not supported for SOCK_STREAM sockets.
 *
 * Usage:
 *
 * // create a udp server
 * int sockfd = udp_socket();
 *
 * // set recvttl option
 * net_recvttl(sockfd, 1);
 *
 * // receive data with control message
 * num_bytes = recvmsg(sockfd, &msg, 0);
 *
 * // extract ttl in control message
 * for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
 *   if (cmsg->cmsg_level == IPPROTO_IP && cmsg->cmsg_type == IP_TTL)
 *     ttl = (int)*CMSG_DATA(cmsg);
 *   if (cmsg->cmsg_level == IPPROTO_IP && cmsg->cmsg_type == IP_TOS)
 *     tos = (int)*CMSG_DATA(cmsg);
 * }
 *
 */
int net_recvttl(int sockfd, int on);
int net_recvtos(int sockfd, int on);
int net_pktinfo(int sockfd, int on);
int net_options(int sockfd, void* opt, int optlen);
int net_recvopts(int sockfd, int on);
// send
// if = auto choose
// ttl = 1
// loop = true
int net_ipv4_multicast(int sock, int optname, struct in_addr if_addr,
                       unsigned int mcast_addr, ifindex_t ifindex);

/*---------------------------------------------------------------------------
                            high-level client-server APIs
----------------------------------------------------------------------------*/
/* FD to address string conversion types */
#define FD_TO_PEER_NAME 0
#define FD_TO_SOCK_NAME 1
// getsockname
// getpeername
// setsockname => bind
// setpeername => connect

int net_resolve(char* err, char* host, char* ipbuf, size_t ipbuf_len, int flags);
int net_tcp_connect(char* err, const char* addr, int port);
int net_tcp_nonblock_connect(char* err, const char* addr, int port);
int net_tcp_nb_be_bind_connect(char* err, const char* addr, int port, const char* source_addr);
int net_tcp_server(char* err, int port, char* bindaddr, int backlog);
int net_tcp6_server(char* err, int port, char* bindaddr, int backlog);
int net_unix_server(char* err, char* path, mode_t perm, int backlog);
int net_tcp_accept(char* err, int serversock, char* ip, size_t ip_len, int* port);
int net_unix_accept(char* err, int serversock);
int net_udp_server(char* err, int port, char* bindaddr);
int net_udp6_server(char* err, int port, char* bindaddr);
int net_udp_client(char* err, int port, const char* addr, struct sockaddr** saptr, socklen_t* lenp);
int net_udp_connect(char* err, int port, const char* addr);

int net_fd_to_string(int fd, char* ip, size_t ip_len, int* port, int fd_to_str_type);
int net_format_addr(char* fmt, size_t fmt_len, char* ip, int port);
int net_format_fd_addr(int fd, char* buf, size_t buf_len, int fd_to_str_type);

ssize_t net_readline(int fd, void* vptr, size_t maxlen);

// ssize_t net_sendmsg(int fd, const struct msghdr *msg, int flags);

// net_readn
// net_read_until_xx
// TODO: libhv

#ifdef __cplusplus
}
#endif
#endif
