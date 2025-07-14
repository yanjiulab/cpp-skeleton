
#include "libanet.h"
#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------
                            utils
----------------------------------------------------------------------------*/
static void net_error(char* err, const char* fmt, ...) {
    va_list ap;

    if (!err) return;
    va_start(ap, fmt);
    vsnprintf(err, ANET_ERR_LEN, fmt, ap);
    va_end(ap);
}

/*---------------------------------------------------------------------------
                            socket options
----------------------------------------------------------------------------*/
#define SOCK_OPT
static int net_set_block(char* err, int fd, int non_block) {
    int flags;

    /* Set the socket blocking (if non_block is zero) or non-blocking.
     * Note that fcntl(2) for F_GETFL and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags = fcntl(fd, F_GETFL)) == -1) {
        net_error(err, "fcntl(F_GETFL): %s", strerror(errno));
        return ANET_ERR;
    }

    /* Check if this flag has been set or unset, if so,
     * then there is no need to call fcntl to set/unset it again. */
    if (!!(flags & O_NONBLOCK) == !!non_block)
        return ANET_OK;

    if (non_block)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags) == -1) {
        net_error(err, "fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

int net_nonblock(char* err, int fd) {
    return net_set_block(err, fd, 1);
}

int net_block(char* err, int fd) {
    return net_set_block(err, fd, 0);
}

/* Enable the FD_CLOEXEC on the given fd to avoid fd leaks.
 * This function should be invoked for fd's on specific places
 * where fork + execve system calls are called. */
int net_cloexec(int fd) {
    int r;
    int flags;

    do {
        r = fcntl(fd, F_GETFD);
    } while (r == -1 && errno == EINTR);

    if (r == -1 || (r & FD_CLOEXEC))
        return r;

    flags = r | FD_CLOEXEC;

    do {
        r = fcntl(fd, F_SETFD, flags);
    } while (r == -1 && errno == EINTR);

    return r;
}

/* Set TCP keep alive option to detect dead peers. The interval option
 * is only used for Linux as we are using Linux-specific APIs to set
 * the probe send time, interval, and count. */
int net_keepalive(char* err, int fd, int interval) {
    int val = 1;

    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) == -1) {
        net_error(err, "setsockopt SO_KEEPALIVE: %s", strerror(errno));
        return ANET_ERR;
    }

#ifdef __linux__
    /* Default settings are more or less garbage, with the keepalive time
     * set to 7200 by default on Linux. Modify settings to make the feature
     * actually useful. */

    /* Send first probe after interval. */
    val = interval;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val)) < 0) {
        net_error(err, "setsockopt TCP_KEEPIDLE: %s\n", strerror(errno));
        return ANET_ERR;
    }

    /* Send next probes after the specified interval. Note that we set the
     * delay as interval / 3, as we send three probes before detecting
     * an error (see the next setsockopt call). */
    val = interval / 3;
    if (val == 0) val = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val)) < 0) {
        net_error(err, "setsockopt TCP_KEEPINTVL: %s\n", strerror(errno));
        return ANET_ERR;
    }

    /* Consider the socket in error state after three we send three ACK
     * probes without getting a reply. */
    val = 3;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &val, sizeof(val)) < 0) {
        net_error(err, "setsockopt TCP_KEEPCNT: %s\n", strerror(errno));
        return ANET_ERR;
    }
#else
    ((void)interval); /* Avoid unused var warning for non Linux systems. */
#endif

    return ANET_OK;
}

static int net_set_tcp_nodelay(char* err, int fd, int val) {
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)) == -1) {
        net_error(err, "setsockopt TCP_NODELAY: %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

int net_enable_tcp_nodelay(char* err, int fd) {
    return net_set_tcp_nodelay(err, fd, 1);
}

int net_disable_tcp_nodelay(char* err, int fd) {
    return net_set_tcp_nodelay(err, fd, 0);
}

static int net_set_tcp_nopush(char* err, int fd, int val) {
    if (setsockopt(fd, IPPROTO_TCP, TCP_CORK, &val, sizeof(val)) == -1) {
        net_error(err, "setsockopt TCP_CORK: %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

int net_enable_tcp_nopush(char* err, int fd) {
    return net_set_tcp_nopush(err, fd, 1);
}

int net_disable_tcp_nopush(char* err, int fd) {
    return net_set_tcp_nopush(err, fd, 0);
}

/* Set the socket send timeout (SO_SNDTIMEO socket option) to the specified
 * number of milliseconds, or disable it if the 'ms' argument is zero. */
int net_send_timeout(char* err, int fd, long long ms) {
    struct timeval tv;

    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1) {
        net_error(err, "setsockopt SO_SNDTIMEO: %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

/* Set the socket receive timeout (SO_RCVTIMEO socket option) to the specified
 * number of milliseconds, or disable it if the 'ms' argument is zero. */
int net_recv_timeout(char* err, int fd, long long ms) {
    struct timeval tv;

    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        net_error(err, "setsockopt SO_RCVTIMEO: %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

// 1. 0.0.0.0:21, 192.168.1.1:21 => failed if SO_REUSEADDR not set
// 2. reconnect on server TIME_WAIT => failed if SO_REUSEADDR not set
int net_set_reuse_addr(char* err, int fd) {
    int yes = 1;
    /* Make sure connection-intensive things like the redis benchmark
     * will be able to close/open sockets a zillion of times */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        net_error(err, "setsockopt SO_REUSEADDR: %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

// SO_REUSEPORT allow multiple sockets to bind same port in load balance mode (linux >= 3.9)
int net_set_reuse_port(char* err, int fd) {
#ifdef SO_REUSEPORT
    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) == -1) {
        net_error(err, "setsockopt SO_REUSEPORT: %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;

#else
    return ANET_OK;
#endif /* 0 */
}

int net_attach_filter(char* err, int fd, struct sock_fprog fprog) {
#ifdef OS_LINUX
    if (setsockopt(fd, SOL_SOCKET, SO_ATTACH_FILTER, &fprog, sizeof(fprog)) == -1) {
        net_error(err, "setsockopt SO_ATTACH_FILTER: %s", strerror(errno));
        return ANET_ERR;
    }
#endif // OS_LINUX
    return ANET_OK;
}

static int net_set_udp_broadcast(char* err, int fd, int on) {
    if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof(int)) == -1) {
        net_error(err, "setsockopt SO_BROADCAST: %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

int net_enable_udp_broadcast(char* err, int fd) {
    return net_set_udp_broadcast(err, fd, 1);
}

int net_disable_udp_broadcast(char* err, int fd) {
    return net_set_udp_broadcast(err, fd, 0);
}

int net_bindtodev(char* err, int fd, char* ifname) {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);

    if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, (char*)&ifr, sizeof(ifr)) == -1) {
        net_error(err, "setsockopt SO_BINDTODEVICE: %s", strerror(errno));
        return ANET_ERR;
    }

    return ANET_OK;
}

int net_set_send_tos(char* err, int sockfd, int tos) {
    if (setsockopt(sockfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos)) < 0) {
        net_error(err, "setsockopt IP_TOS: %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

int net_get_send_tos(char* err, int sockfd) {
    int tos;
    socklen_t len = sizeof(int);
    if (getsockopt(sockfd, IPPROTO_IP, IP_TOS, &tos, &len) < 0) {
        net_error(err, "getsockopt IP_TOS: %s", strerror(errno));
        return ANET_ERR;
    }
    return tos;
}

int net_set_send_ttl(char* err, int family, int sock, int ttl) {
    int ret;

#ifdef IP_TTL
    if (family == AF_INET) {
        ret = setsockopt(sock, IPPROTO_IP, IP_TTL, (void*)&ttl,
                         sizeof(int));
        if (ret < 0) {
            net_error(err, "setsockopt IP_TTL: %s", strerror(errno));
            return ANET_ERR;
        }
        return ANET_OK;
    }
#endif /* IP_TTL */
    if (family == AF_INET6) {
        ret = setsockopt(sock, IPPROTO_IPV6, IPV6_UNICAST_HOPS,
                         (void*)&ttl, sizeof(int));
        if (ret < 0) {
            net_error(err, "setsockopt IP_TTL: %s", strerror(errno));
            return ANET_ERR;
        }
        return ANET_OK;
    }
    return ANET_OK;
}

int net_get_send_ttl(char* err, int sockfd) {
    int ttl;
    socklen_t len = sizeof(int);
    if (getsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, &len) < 0) {
        net_error(err, "getsockopt IP_TTL: %s", strerror(errno));
        return ANET_ERR;
    }
    return ttl;
}

int so_set_minttl(char* err, int family, int sock, int minttl) {
#ifdef IP_MINTTL
    if (family == AF_INET) {
        int ret = setsockopt(sock, IPPROTO_IP, IP_MINTTL, &minttl,
                             sizeof(minttl));
        if (ret < 0) {
            net_error(err, "setsockopt IP_MINTTL: %s", strerror(errno));
            return ANET_ERR;
        }

        return ret;
    }
#endif /* IP_MINTTL */
#ifdef IPV6_MINHOPCOUNT
    if (family == AF_INET6) {
        int ret = setsockopt(sock, IPPROTO_IPV6, IPV6_MINHOPCOUNT,
                             &minttl, sizeof(minttl));
        if (ret < 0) {
            net_error(err, "setsockopt IPV6_MINHOPCOUNT: %s", strerror(errno));
            return ANET_ERR;
        }
        return ret;
    }
#endif

    errno = EOPNOTSUPP;
    return ANET_ERR;
}

static int net_set_socket_buffer_size(int fd, int bufsize, int type) {
    int delta = bufsize / 2;
    int iter = 0;

    /* Set the socket buffer.  If we can't set it as large as we want, search around
     * to try to find the highest acceptable value. */
    if (setsockopt(fd, SOL_SOCKET, type, (char*)&bufsize, sizeof(bufsize)) < 0) {
        bufsize -= delta;
        while (1) {
            iter++;
            if (delta > 1) delta /= 2;
            if (setsockopt(fd, SOL_SOCKET, type, (char*)&bufsize, sizeof(bufsize)) < 0) {
                bufsize -= delta;
                printd("%s",strerror(errno));
            } else {
                if (delta < 1024) break;
                bufsize += delta;
            }
        }
    }

    return setsockopt(fd, SOL_SOCKET, type, (const char*)&bufsize, sizeof(int));
}

int net_recvbuf(int fd, int bufsize) {
    return net_set_socket_buffer_size(fd, bufsize, SO_RCVBUF);
}

int net_sendbuf(int fd, int bufsize) {
    return net_set_socket_buffer_size(fd, bufsize, SO_SNDBUF);
}

int net_linger(int sockfd, int timeout) {
#ifdef SO_LINGER
    struct linger linger;
    if (timeout >= 0) {
        linger.l_onoff = 1;
        linger.l_linger = timeout;
    } else {
        linger.l_onoff = 0;
        linger.l_linger = 0;
    }
    // NOTE: SO_LINGER change the default behavior of close, send RST, avoid
    // TIME_WAIT
    return setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (const char*)&linger,
                      sizeof(linger));
#else
    return 0;
#endif
}

int net_noroute(int sockfd, int on) {
    return setsockopt(sockfd, SOL_SOCKET, SO_DONTROUTE, (const char*)&on, sizeof(int));
}

int net_recvttl(int sockfd, int on) {
    if (setsockopt(sockfd, IPPROTO_IP, IP_RECVTTL, &on, sizeof(on)) < 0) {
        printe("can't setsockopt IP_RECVTTL on fd %d for TTL reception\n", sockfd);
        return -1;
    }
    return 0;
}

int net_recvtos(int sockfd, int on) {
    if (setsockopt(sockfd, IPPROTO_IP, IP_RECVTOS, &on, sizeof(on)) < 0) {
        printe("can't setsockopt IP_RECVTOS on fd %d for TOS reception\n", sockfd);
        return -1;
    }
    return 0;
}

int net_pktinfo(int sockfd, int on) {
    if (setsockopt(sockfd, IPPROTO_IP, IP_PKTINFO, &on, sizeof(on)) < 0) {
        printe("can't setsockopt IP_PKTINFO on fd %d PKTINFO reception\n", sockfd);
        return -1;
    }
    return 0;
}

int net_options(int sockfd, void* opt, int optlen) {
    if (setsockopt(sockfd, IPPROTO_IP, IP_OPTIONS, opt, optlen) < 0) {
        printe("can't setsockopt IP_OPTIONS on fd %d: %s\n", sockfd, strerror(errno));
        return -1;
    }
    return 0;
}

int net_recvopts(int sockfd, int on) {
    if (setsockopt(sockfd, IPPROTO_IP, IP_RECVOPTS, &on, sizeof(on)) < 0) {
        printe("can't setsockopt IP_RECVOPTS on fd %d for IP_RECVOPTS reception\n", sockfd);
        return -1;
    }
    return 0;
}

/*
 * Process multicast socket options for IPv4 in an OS-dependent manner.
 * Supported options are IP_{ADD,DROP}_MEMBERSHIP.
 *
 * Many operating systems have a limit on the number of groups that
 * can be joined per socket (where each group and local address
 * counts).  This impacts OSPF, which joins groups on each interface
 * using a single socket.  The limit is typically 20, derived from the
 * original BSD multicast implementation.  Some systems have
 * mechanisms for increasing this limit.
 *
 * In many 4.4BSD-derived systems, multicast group operations are not
 * allowed on interfaces that are not UP.  Thus, a previous attempt to
 * leave the group may have failed, leaving it still joined, and we
 * drop/join quietly to recover.  This may not be necessary, but aims to
 * defend against unknown behavior in that we will still return an error
 * if the second join fails.  It is not clear how other systems
 * (e.g. Linux, Solaris) behave when leaving groups on down interfaces,
 * but this behavior should not be harmful if they behave the same way,
 * allow leaves, or implicitly leave all groups joined to down interfaces.
 */
int net_ipv4_multicast(int sock, int optname, struct in_addr if_addr,
                      unsigned int mcast_addr, ifindex_t ifindex) {
    // #ifdef HAVE_RFC3678
    struct group_req gr;
    struct sockaddr_in* si;
    int ret;
    memset(&gr, 0, sizeof(gr));
    si = (struct sockaddr_in*)&gr.gr_group;
    gr.gr_interface = ifindex;
    si->sin_family = AF_INET;
#ifdef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
    si->sin_len = sizeof(struct sockaddr_in);
#endif /* HAVE_STRUCT_SOCKADDR_IN_SIN_LEN */
    si->sin_addr.s_addr = mcast_addr;
    ret = setsockopt(sock, IPPROTO_IP,
                     (optname == IP_ADD_MEMBERSHIP) ? MCAST_JOIN_GROUP : MCAST_LEAVE_GROUP,
                     (void*)&gr, sizeof(gr));
    if ((ret < 0) && (optname == IP_ADD_MEMBERSHIP) && (errno == EADDRINUSE)) {
        setsockopt(sock, IPPROTO_IP, MCAST_LEAVE_GROUP, (void*)&gr, sizeof(gr));
        ret = setsockopt(sock, IPPROTO_IP, MCAST_JOIN_GROUP, (void*)&gr, sizeof(gr));
    }

    if (ret < 0) {
        printe("can't setsockopt IP_ADD_MEMBERSHIP %s\n", strerror(errno));
    }

    return ret;

    // #elif defined(HAVE_STRUCT_IP_MREQN_IMR_IFINDEX) && !defined(__FreeBSD__)
    // struct ip_mreqn mreqn;
    // 	int ret;

    // 	assert(optname == IP_ADD_MEMBERSHIP || optname == IP_DROP_MEMBERSHIP);
    // 	memset(&mreqn, 0, sizeof(mreqn));

    // 	mreqn.imr_multiaddr.s_addr = mcast_addr;
    // 	mreqn.imr_ifindex = ifindex;

    // 	ret = setsockopt(sock, IPPROTO_IP, optname, (void *)&mreqn,
    // 			 sizeof(mreqn));
    // 	if ((ret < 0) && (optname == IP_ADD_MEMBERSHIP)
    // 	    && (errno == EADDRINUSE)) {
    // 		/* see above: handle possible problem when interface comes back
    // 		 * up */
    // 		zlog_info(
    // 			"so_ipv4_multicast attempting to drop and re-add (fd %d, mcast %pI4, ifindex %u)",
    // 			sock, &mreqn.imr_multiaddr, ifindex);
    // 		setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void *)&mreqn,
    // 			   sizeof(mreqn));
    // 		ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
    // 				 (void *)&mreqn, sizeof(mreqn));
    // 	}
    // 	return ret;

    // /* Example defines for another OS, boilerplate off other code in this
    //    function, AND handle optname as per other sections for consistency !! */
    // /* #elif  defined(BOGON_NIX) && EXAMPLE_VERSION_CODE > -100000 */
    // /* Add your favourite OS here! */

    // #elif defined(HAVE_BSD_STRUCT_IP_MREQ_HACK) /* #if OS_TYPE */
    // 	/* standard BSD API */

    // struct ip_mreq mreq;
    // 	int ret;

    // 	assert(optname == IP_ADD_MEMBERSHIP || optname == IP_DROP_MEMBERSHIP);

    // 	memset(&mreq, 0, sizeof(mreq));
    // 	mreq.imr_multiaddr.s_addr = mcast_addr;
    // #if !defined __OpenBSD__
    // 	mreq.imr_interface.s_addr = htonl(ifindex);
    // #else
    // 	mreq.imr_interface.s_addr = if_addr.s_addr;
    // #endif

    // 	ret = setsockopt(sock, IPPROTO_IP, optname, (void *)&mreq,
    // 			 sizeof(mreq));
    // 	if ((ret < 0) && (optname == IP_ADD_MEMBERSHIP)
    // 	    && (errno == EADDRINUSE)) {
    // 		/* see above: handle possible problem when interface comes back
    // 		 * up */
    // 		zlog_info(
    // 			"so_ipv4_multicast attempting to drop and re-add (fd %d, mcast %pI4, ifindex %u)",
    // 			sock, &mreq.imr_multiaddr, ifindex);
    // 		setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void *)&mreq,
    // 			   sizeof(mreq));
    // 		ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
    // 				 (void *)&mreq, sizeof(mreq));
    // 	}
    // 	return ret;

    // #else
    // #error "Unsupported multicast API"
    // #endif /* #if OS_TYPE */
}

/*---------------------------------------------------------------------------
                            high-level client-server APIs
----------------------------------------------------------------------------*/

/* Resolve the hostname "host" and set the string representation of the
 * IP address into the buffer pointed by "ipbuf".
 *
 * If flags is set to ANET_IP_ONLY the function only resolves hostnames
 * that are actually already IPv4 or IPv6 addresses. This turns the function
 * into a validating / normalizing function. */
int net_resolve(char* err, char* host, char* ipbuf, size_t ipbuf_len,
                int flags) {
    struct addrinfo hints, *info;
    int rv;

    memset(&hints, 0, sizeof(hints));
    if (flags & ANET_IP_ONLY) hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; /* specify socktype to avoid dups */

    if ((rv = getaddrinfo(host, NULL, &hints, &info)) != 0) {
        net_error(err, "%s", gai_strerror(rv));
        return ANET_ERR;
    }
    if (info->ai_family == AF_INET) {
        struct sockaddr_in* sa = (struct sockaddr_in*)info->ai_addr;
        inet_ntop(AF_INET, &(sa->sin_addr), ipbuf, ipbuf_len);
    } else {
        struct sockaddr_in6* sa = (struct sockaddr_in6*)info->ai_addr;
        inet_ntop(AF_INET6, &(sa->sin6_addr), ipbuf, ipbuf_len);
    }

    freeaddrinfo(info);
    return ANET_OK;
}

static int net_create_socket(char* err, int domain) {
    int s;
    if ((s = socket(domain, SOCK_STREAM, 0)) == -1) {
        net_error(err, "creating socket: %s", strerror(errno));
        return ANET_ERR;
    }

    /* Make sure connection-intensive things like the redis benchmark
     * will be able to close/open sockets a zillion of times */
    if (net_set_reuse_addr(err, s) == ANET_ERR) {
        close(s);
        return ANET_ERR;
    }
    return s;
}

#define ANET_CONNECT_NONE       0
#define ANET_CONNECT_NONBLOCK   1
#define ANET_CONNECT_BE_BINDING 2 /* Best effort binding. */
static int net_tcp_generic_connect(char* err, const char* addr, int port,
                                   const char* source_addr, int flags) {
    int s = ANET_ERR, rv;
    char portstr[6]; /* strlen("65535") + 1; */
    struct addrinfo hints, *servinfo, *bservinfo, *p, *b;

    snprintf(portstr, sizeof(portstr), "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(addr, portstr, &hints, &servinfo)) != 0) {
        net_error(err, "%s", gai_strerror(rv));
        return ANET_ERR;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        /* Try to create the socket and to connect it.
         * If we fail in the socket() call, or on connect(), we retry with
         * the next entry in servinfo. */
        if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;
        if (net_set_reuse_addr(err, s) == ANET_ERR) goto error;
        if (flags & ANET_CONNECT_NONBLOCK && net_nonblock(err, s) != ANET_OK)
            goto error;
        if (source_addr) {
            int bound = 0;
            /* Using getaddrinfo saves us from self-determining IPv4 vs IPv6 */
            if ((rv = getaddrinfo(source_addr, NULL, &hints, &bservinfo)) != 0) {
                net_error(err, "%s", gai_strerror(rv));
                goto error;
            }
            for (b = bservinfo; b != NULL; b = b->ai_next) {
                if (bind(s, b->ai_addr, b->ai_addrlen) != -1) {
                    bound = 1;
                    break;
                }
            }
            freeaddrinfo(bservinfo);
            if (!bound) {
                net_error(err, "bind: %s", strerror(errno));
                goto error;
            }
        }
        if (connect(s, p->ai_addr, p->ai_addrlen) == -1) {
            /* If the socket is non-blocking, it is ok for connect() to
             * return an EINPROGRESS error here. */
            if (errno == EINPROGRESS && flags & ANET_CONNECT_NONBLOCK) {
                goto end;
            }
            close(s);
            s = ANET_ERR;
            continue;
        }

        /* If we ended an iteration of the for loop without errors, we
         * have a connected socket. Let's return to the caller. */
        goto end;
    }
    if (p == NULL)
        net_error(err, "creating socket: %s", strerror(errno));

error:
    if (s != ANET_ERR) {
        close(s);
        s = ANET_ERR;
    }

end:
    freeaddrinfo(servinfo);

    /* Handle best effort binding: if a binding address was used, but it is
     * not possible to create a socket, try again without a binding address. */
    if (s == ANET_ERR && source_addr && (flags & ANET_CONNECT_BE_BINDING)) {
        return net_tcp_generic_connect(err, addr, port, NULL, flags);
    } else {
        return s;
    }
}

int net_tcp_connect(char* err, const char* addr, int port) {
    return net_tcp_generic_connect(err, addr, port, NULL, ANET_CONNECT_NONE);
}

int net_tcp_nonblock_connect(char* err, const char* addr, int port) {
    return net_tcp_generic_connect(err, addr, port, NULL, ANET_CONNECT_NONBLOCK);
}

int net_tcp_nb_be_bind_connect(char* err, const char* addr, int port,
                               const char* source_addr) {
    return net_tcp_generic_connect(err, addr, port, source_addr,
                                   ANET_CONNECT_NONBLOCK | ANET_CONNECT_BE_BINDING);
}

int net_unix_generic_connect(char* err, const char* path, int flags) {
    int s;
    struct sockaddr_un sa;

    if ((s = net_create_socket(err, AF_LOCAL)) == ANET_ERR)
        return ANET_ERR;

    sa.sun_family = AF_LOCAL;
    strncpy(sa.sun_path, path, sizeof(sa.sun_path) - 1);
    if (flags & ANET_CONNECT_NONBLOCK) {
        if (net_nonblock(err, s) != ANET_OK) {
            close(s);
            return ANET_ERR;
        }
    }
    if (connect(s, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
        if (errno == EINPROGRESS &&
            flags & ANET_CONNECT_NONBLOCK)
            return s;

        net_error(err, "connect: %s", strerror(errno));
        close(s);
        return ANET_ERR;
    }
    return s;
}

static int net_bind(char* err, int s, struct sockaddr* sa, socklen_t len) {
    if (bind(s, sa, len) == -1) {
        net_error(err, "bind: %s", strerror(errno));
        close(s);
        return ANET_ERR;
    }
    return ANET_OK;
}

static int net_listen(char* err, int s, struct sockaddr* sa, socklen_t len, int backlog) {
    if (net_bind(err, s, sa, len) == ANET_ERR)
        return ANET_ERR;

    if (listen(s, backlog) == -1) {
        net_error(err, "listen: %s", strerror(errno));
        close(s);
        return ANET_ERR;
    }
    return ANET_OK;
}

static int net_v6_only(char* err, int s) {
    int yes = 1;
    if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(yes)) == -1) {
        net_error(err, "setsockopt: %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

static int _net_tcp_server(char* err, int port, char* bindaddr, int af, int backlog) {
    int s = -1, rv;
    char _port[6]; /* strlen("65535") */
    struct addrinfo hints, *servinfo, *p;

    snprintf(_port, 6, "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = af;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; /* No effect if bindaddr != NULL */
    if (bindaddr && !strcmp("*", bindaddr))
        bindaddr = NULL;
    if (af == AF_INET6 && bindaddr && !strcmp("::*", bindaddr))
        bindaddr = NULL;

    if ((rv = getaddrinfo(bindaddr, _port, &hints, &servinfo)) != 0) {
        net_error(err, "%s", gai_strerror(rv));
        return ANET_ERR;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (af == AF_INET6 && net_v6_only(err, s) == ANET_ERR) goto error;
        if (net_set_reuse_addr(err, s) == ANET_ERR) goto error;
        if (net_set_reuse_port(err, s) == ANET_ERR) goto error;
        if (net_listen(err, s, p->ai_addr, p->ai_addrlen, backlog) == ANET_ERR) s = ANET_ERR;
        goto end;
    }
    if (p == NULL) {
        net_error(err, "unable to bind socket, errno: %d", errno);
        goto error;
    }

error:
    if (s != -1) close(s);
    s = ANET_ERR;
end:
    freeaddrinfo(servinfo);
    return s;
}

int net_tcp_server(char* err, int port, char* bindaddr, int backlog) {
    return _net_tcp_server(err, port, bindaddr, AF_INET, backlog);
}

int net_tcp6_server(char* err, int port, char* bindaddr, int backlog) {
    return _net_tcp_server(err, port, bindaddr, AF_INET6, backlog);
}

int net_unix_server(char* err, char* path, mode_t perm, int backlog) {
    int s;
    struct sockaddr_un sa;

    if ((s = net_create_socket(err, AF_LOCAL)) == ANET_ERR)
        return ANET_ERR;

    memset(&sa, 0, sizeof(sa));
    sa.sun_family = AF_LOCAL;
    strncpy(sa.sun_path, path, sizeof(sa.sun_path) - 1);
    if (net_listen(err, s, (struct sockaddr*)&sa, sizeof(sa), backlog) == ANET_ERR)
        return ANET_ERR;
    if (perm)
        chmod(sa.sun_path, perm);
    return s;
}

static int net_generic_accept(char* err, int s, struct sockaddr* sa, socklen_t* len) {
    int fd;
    while (1) {
        fd = accept(s, sa, len);
        if (fd == -1) {
            if (errno == EINTR)
                continue;
            else {
                net_error(err, "accept: %s", strerror(errno));
                return ANET_ERR;
            }
        }
        break;
    }
    return fd;
}

int net_tcp_accept(char* err, int s, char* ip, size_t ip_len, int* port) {
    int fd;
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);
    if ((fd = net_generic_accept(err, s, (struct sockaddr*)&sa, &salen)) == -1)
        return ANET_ERR;

    if (sa.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in*)&sa;
        if (ip) inet_ntop(AF_INET, (void*)&(s->sin_addr), ip, ip_len);
        if (port) *port = ntohs(s->sin_port);
    } else {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&sa;
        if (ip) inet_ntop(AF_INET6, (void*)&(s->sin6_addr), ip, ip_len);
        if (port) *port = ntohs(s->sin6_port);
    }
    return fd;
}

int net_unix_accept(char* err, int s) {
    int fd;
    struct sockaddr_un sa;
    socklen_t salen = sizeof(sa);
    if ((fd = net_generic_accept(err, s, (struct sockaddr*)&sa, &salen)) == -1)
        return ANET_ERR;

    return fd;
}

static int _net_udp_server(char* err, int port, char* bindaddr, int af) {
    int s = -1, rv;
    char _port[6]; /* strlen("65535") */
    struct addrinfo hints, *servinfo, *p;
    // struct addrinfo hints, *res, *ressave;

    snprintf(_port, 6, "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = af;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    /* No effect if bindaddr != NULL */
    if (bindaddr && !strcmp("*", bindaddr))
        bindaddr = NULL;
    if (af == AF_INET6 && bindaddr && !strcmp("::*", bindaddr))
        bindaddr = NULL;

    if ((rv = getaddrinfo(bindaddr, _port, &hints, &servinfo)) != 0) {
        net_error(err, "%s", gai_strerror(rv));
        return ANET_ERR;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (af == AF_INET6 && net_v6_only(err, s) == ANET_ERR) goto error;
        if (net_set_reuse_addr(err, s) == ANET_ERR) goto error;
        if (net_set_reuse_port(err, s) == ANET_ERR) goto error;
        if (net_bind(err, s, p->ai_addr, p->ai_addrlen) == ANET_ERR) s = ANET_ERR;
        goto end;
    }
    if (p == NULL) {
        net_error(err, "unable to bind socket, errno: %d", errno);
        goto error;
    }

error:
    if (s != -1) close(s);
    s = ANET_ERR;
end:
    freeaddrinfo(servinfo);
    return s;
}

int net_udp_server(char* err, int port, char* bindaddr) {
    return _net_udp_server(err, port, bindaddr, AF_INET);
}

int net_udp6_server(char* err, int port, char* bindaddr) {
    return _net_udp_server(err, port, bindaddr, AF_INET6);
}

int net_udp_client(char* err, int port, const char* addr, struct sockaddr** saptr, socklen_t* lenp) {
    int s = -1, rv;
    char portstr[6]; /* strlen("65535") + 1; */
    // struct addrinfo hints, *res, *ressave;
    struct addrinfo hints, *servinfo, *p;

    snprintf(portstr, sizeof(portstr), "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(addr, portstr, &hints, &servinfo)) != 0) {
        net_error(err, "%s", gai_strerror(rv));
        return ANET_ERR;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        /* Try to create the socket and to connect it.
         * If we fail in the socket() call, or on connect(), we retry with
         * the next entry in servinfo. */
        if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) >= 0)
            break;
    }
    if (p == NULL)
        net_error(err, "creating socket: %s", strerror(errno));

    *saptr = malloc(servinfo->ai_addrlen);
    memcpy(*saptr, servinfo->ai_addr, servinfo->ai_addrlen);
    *lenp = servinfo->ai_addrlen;

    freeaddrinfo(servinfo);

    return (s);
}

int net_udp_connect(char* err, int port, const char* addr) {
    int s = -1, rv;
    char portstr[6]; /* strlen("65535") + 1; */
    // struct addrinfo hints, *res, *ressave;
    struct addrinfo hints, *servinfo, *p;

    snprintf(portstr, sizeof(portstr), "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(addr, portstr, &hints, &servinfo)) != 0) {
        net_error(err, "%s", gai_strerror(rv));
        return ANET_ERR;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        /* Try to create the socket and to connect it.
         * If we fail in the socket() call, or on connect(), we retry with
         * the next entry in servinfo. */
        if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue;

        if (connect(s, p->ai_addr, p->ai_addrlen) == 0)
            break; /* success */
        close(s);  /* ignore this one */
    }
    if (p == NULL)
        net_error(err, "creating socket: %s", strerror(errno));

    freeaddrinfo(servinfo);

    return (s);
}

int net_fd_to_string(int fd, char* ip, size_t ip_len, int* port, int fd_to_str_type) {
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);

    if (fd_to_str_type == FD_TO_PEER_NAME) {
        if (getpeername(fd, (struct sockaddr*)&sa, &salen) == -1) goto error;
    } else {
        if (getsockname(fd, (struct sockaddr*)&sa, &salen) == -1) goto error;
    }
    if (ip_len == 0) goto error;

    if (sa.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in*)&sa;
        if (ip) inet_ntop(AF_INET, (void*)&(s->sin_addr), ip, ip_len);
        if (port) *port = ntohs(s->sin_port);
    } else if (sa.ss_family == AF_INET6) {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&sa;
        if (ip) inet_ntop(AF_INET6, (void*)&(s->sin6_addr), ip, ip_len);
        if (port) *port = ntohs(s->sin6_port);
    } else if (sa.ss_family == AF_UNIX) {
        if (ip) snprintf(ip, ip_len, "/unixsocket");
        if (port) *port = 0;
    } else {
        goto error;
    }
    return 0;

error:
    if (ip) {
        if (ip_len >= 2) {
            ip[0] = '?';
            ip[1] = '\0';
        } else if (ip_len == 1) {
            ip[0] = '\0';
        }
    }
    if (port) *port = 0;
    return -1;
}

/* Format an IP,port pair into something easy to parse. If IP is IPv6
 * (matches for ":"), the ip is surrounded by []. IP and port are just
 * separated by colons. This the standard to display addresses within Redis. */
int net_format_addr(char* buf, size_t buf_len, char* ip, int port) {
    return snprintf(buf, buf_len, strchr(ip, ':') ? "[%s]:%d" : "%s:%d", ip, port);
}

/* Like net_format_addr() but extract ip and port from the socket's peer/sockname. */
int net_format_fd_addr(int fd, char* buf, size_t buf_len, int fd_to_str_type) {
    char ip[INET6_ADDRSTRLEN];
    int port;

    net_fd_to_string(fd, ip, sizeof(ip), &port, fd_to_str_type);
    return net_format_addr(buf, buf_len, ip, port);
}


ssize_t net_readline(int fd, void* vptr, size_t maxlen) {
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
    again:
        if ((rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break; /* newline is stored, like fgets() */
        } else if (rc == 0) {
            *ptr = 0;
            return (n - 1); /* EOF, n - 1 bytes were read */
        } else {
            if (errno == EINTR)
                goto again;
            return (-1); /* error, errno set by read() */
        }
    }

    *ptr = 0; /* null terminate like fgets() */
    return (n);
}

// ssize_t net_sendmsg(int fd, const struct msghdr *msg, int flags) {

// }