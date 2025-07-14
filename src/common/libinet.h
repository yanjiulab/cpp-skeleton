
#ifndef LIBINET_H
#define LIBINET_H

#define LIBINET_VERSION "0.0.1"

#include <stdint.h>
#include <stdbool.h>

#define __USE_MISC 1
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct ethhdr ethhdr_t;      // Ethernet header
typedef struct arphdr arphdr_t;      // ARP header
typedef struct ether_arp arp_t;      // ARP packet
typedef struct iphdr iphdr_t;        // IPv4 header
typedef struct ip6_hdr ip6hdr_t;     // IPv6 header
typedef struct udphdr udphdr_t;      // UDP header
typedef struct tcphdr tcphdr_t;      // TCP header
typedef struct icmphdr icmphdr_t;    // ICMP header
typedef struct icmp icmp_t;          // ICMP packet
typedef struct icmp6_hdr icmp6hdr_t; // ICMPv6 header

typedef struct ether_addr ethaddr_t; // Ethernet address
typedef struct ipaddr ipaddr_t;      // IPv4/IPv6 address
typedef struct prefix prefix_t;      // Generic prefix address
typedef struct endpoint endpoint_t;  // IP:PORT address
typedef struct tuple tuple_t;        // TCP/UDP address

// /* ARP IP-Ethernet protocol header. */
// typedef struct arphdr_s {
//     unsigned short ar_hrd;          /* format of hardware address	*/
//     unsigned short ar_pro;          /* format of protocol address	*/
//     unsigned char ar_hln;           /* length of hardware address	*/
//     unsigned char ar_pln;           /* length of protocol address	*/
//     unsigned short ar_op;           /* ARP opcode (command)		*/
//     unsigned char ar_sha[ETH_ALEN]; /* sender hardware address	*/
//     unsigned char ar_sip[4];        /* sender IP address		*/
//     unsigned char ar_tha[ETH_ALEN]; /* target hardware address	*/
//     unsigned char ar_tip[4];        /* target IP address		*/
// } __attribute__((packed)) arp_t;

// /* ARP protocol opcodes. */
// #define ARPOP_REQUEST   1  /* ARP request.  */
// #define ARPOP_REPLY     2  /* ARP reply.  */
// #define ARPOP_RREQUEST  3  /* RARP request.  */
// #define ARPOP_RREPLY    4  /* RARP reply.  */
// #define ARPOP_InREQUEST 8  /* InARP request.  */
// #define ARPOP_InREPLY   9  /* InARP reply.  */
// #define ARPOP_NAK       10 /* (ATM)ARP NAK.  */

// /* ARP protocol HARDWARE identifiers. */
// #define ARPHRD_ETHER    1  /* Ethernet */

// // sizeof(iphdr_t) = 20
// typedef struct iphdr_s {
// #if BYTE_ORDER == LITTLE_ENDIAN
//     uint8_t ihl : 4; // ip header length
//     uint8_t version : 4;
// #elif BYTE_ORDER == BIG_ENDIAN
//     uint8_t version : 4;
//     uint8_t ihl : 4;
// #else
// #error "BYTE_ORDER undefined!"
// #endif
//     uint8_t tos;      // type of service
//     uint16_t tot_len; // total length
//     uint16_t id;
//     uint16_t frag_off; // fragment offset
//     uint8_t ttl;       // Time To Live
//     uint8_t protocol;
//     uint16_t check; // checksum
//     uint32_t saddr; // srcaddr
//     uint32_t daddr; // dstaddr
//     /*The options start here.*/
// } iphdr_t;

// // sizeof(udphdr_t) = 8
// typedef struct udphdr_s {
//     uint16_t source; // source port
//     uint16_t dest;   // dest   port
//     uint16_t len;    // udp length
//     uint16_t check;  // checksum
// } udphdr_t;

// // sizeof(tcphdr_t) = 20
// typedef struct tcphdr_s {
//     uint16_t source; // source port
//     uint16_t dest;   // dest   port
//     uint32_t seq;    // sequence
//     uint32_t ack_seq;
// #if BYTE_ORDER == LITTLE_ENDIAN
//     uint16_t res1 : 4;
//     uint16_t doff : 4;
//     uint16_t fin : 1;
//     uint16_t syn : 1;
//     uint16_t rst : 1;
//     uint16_t psh : 1;
//     uint16_t ack : 1;
//     uint16_t urg : 1;
//     uint16_t res2 : 2;
// #elif BYTE_ORDER == BIG_ENDIAN
//     uint16_t doff : 4;
//     uint16_t res1 : 4;
//     uint16_t res2 : 2;
//     uint16_t urg : 1;
//     uint16_t ack : 1;
//     uint16_t psh : 1;
//     uint16_t rst : 1;
//     uint16_t syn : 1;
//     uint16_t fin : 1;
// #else
// #error "BYTE_ORDER undefined!"
// #endif
//     uint16_t window;
//     uint16_t check;   // checksum
//     uint16_t urg_ptr; // urgent pointer
// } tcphdr_t;

// //----------------------icmp----------------------------------
// #define ICMP_ECHOREPLY      0  /* Echo Reply			*/
// #define ICMP_DEST_UNREACH   3  /* Destination Unreachable	*/
// #define ICMP_SOURCE_QUENCH  4  /* Source Quench		*/
// #define ICMP_REDIRECT       5  /* Redirect (change route)	*/
// #define ICMP_ECHO           8  /* Echo Request			*/
// #define ICMP_TIME_EXCEEDED  11 /* Time Exceeded		*/
// #define ICMP_PARAMETERPROB  12 /* Parameter Problem		*/
// #define ICMP_TIMESTAMP      13 /* Timestamp Request		*/
// #define ICMP_TIMESTAMPREPLY 14 /* Timestamp Reply		*/
// #define ICMP_INFO_REQUEST   15 /* Information Request		*/
// #define ICMP_INFO_REPLY     16 /* Information Reply		*/
// #define ICMP_ADDRESS        17 /* Address Mask Request		*/
// #define ICMP_ADDRESSREPLY   18 /* Address Mask Reply		*/

// // sizeof(icmphdr_t) = 8
// typedef struct icmphdr_s {
//     uint8_t type; // message type
//     uint8_t code; // type sub-code
//     uint16_t checksum;
//     union {
//         struct {
//             uint16_t id;
//             uint16_t sequence;
//         } echo;
//         uint32_t gateway;
//         struct {
//             uint16_t reserved;
//             uint16_t mtu;
//         } frag;
//     } un;
// } icmphdr_t;

// typedef struct icmp_s {
//     uint8_t icmp_type;
//     uint8_t icmp_code;
//     uint16_t icmp_cksum;
//     union {
//         uint8_t ih_pptr;
//         struct in_addr ih_gwaddr;
//         struct ih_idseq {
//             uint16_t icd_id;
//             uint16_t icd_seq;
//         } ih_idseq;
//         uint32_t ih_void;

//         struct ih_pmtu {
//             uint16_t ipm_void;
//             uint16_t ipm_nextmtu;
//         } ih_pmtu;

//         struct ih_rtradv {
//             uint8_t irt_num_addrs;
//             uint8_t irt_wpa;
//             uint16_t irt_lifetime;
//         } ih_rtradv;
//     } icmp_hun;
// #define icmp_pptr           icmp_hun.ih_pptr
// #define icmp_gwaddr         icmp_hun.ih_gwaddr
// #define icmp_id             icmp_hun.ih_idseq.icd_id
// #define icmp_seq            icmp_hun.ih_idseq.icd_seq
// #define icmp_void           icmp_hun.ih_void
// #define icmp_pmvoid         icmp_hun.ih_pmtu.ipm_void
// #define icmp_nextmtu        icmp_hun.ih_pmtu.ipm_nextmtu
// #define icmp_num_addrs      icmp_hun.ih_rtradv.irt_num_addrs
// #define icmp_wpa            icmp_hun.ih_rtradv.irt_wpa
// #define icmp_lifetime       icmp_hun.ih_rtradv.irt_lifetime

//     union {
//         struct {
//             uint32_t its_otime;
//             uint32_t its_rtime;
//             uint32_t its_ttime;
//         } id_ts;
//         /*
//         struct {
//             struct ip idi_ip;
//         } id_ip;
//         struct icmp_ra_addr id_radv;
//         */
//         uint32_t id_mask;
//         uint8_t id_data[1];
//     } icmp_dun;
// #define icmp_otime          icmp_dun.id_ts.its_otime
// #define icmp_rtime          icmp_dun.id_ts.its_rtime
// #define icmp_ttime          icmp_dun.id_ts.its_ttime
// #define icmp_ip             icmp_dun.id_ip.idi_ip
// #define icmp_radv           icmp_dun.id_radv
// #define icmp_mask           icmp_dun.id_mask
// #define icmp_data           icmp_dun.id_data
// } icmp_t;

/*---------------------------------------------------------------------------
                            Ethernet/MAC Address
----------------------------------------------------------------------------*/

/* IEEE 802.3 Ethernet magic constants. The frame sizes omit the preamble
 * and FCS/CRC (frame check sequence). */
#define ETH_ALEN             6    /* Octets in one ethernet addr	 */
#define ETH_ASLEN            18   /* String in one ethernet addr	 */
#define ETH_TLEN             2    /* Octets in ethernet type field */
#define ETH_HLEN             14   /* Total octets in header.	 */
#define ETH_ZLEN             60   /* Min. octets in frame sans FCS */
#define ETH_DATA_LEN         1500 /* Max. octets in payload	 */
#define ETH_FRAME_LEN        1514 /* Max. octets in frame sans FCS */
#define ETH_FCS_LEN          4    /* Octets in the FCS		 */

/* To tell a packet socket that you want traffic for all protocols. */
#define ETH_P_ALL            0x0003

/* Convert ethernet type to string */
const char* ether_type2str(uint16_t type);

/* Ethernet address operations */
#define ether_copy(dst, src) memcpy(dst, src, sizeof(ethaddr_t))
int ether_str2mac(const char* str, ethaddr_t* mac);
char* ether_mac2str(const ethaddr_t* mac, char* buf, int size);
bool is_zero_mac(const ethaddr_t* mac);
bool is_bcast_mac(const ethaddr_t* mac);
bool is_mcast_mac(const ethaddr_t* mac);

/*---------------------------------------------------------------------------
                            ARP
----------------------------------------------------------------------------*/

/* ARP protocol opcodes. */
#define ARPOP_REQUEST        1  /* ARP request.  */
#define ARPOP_REPLY          2  /* ARP reply.  */
#define ARPOP_RREQUEST       3  /* RARP request.  */
#define ARPOP_RREPLY         4  /* RARP reply.  */
#define ARPOP_InREQUEST      8  /* InARP request.  */
#define ARPOP_InREPLY        9  /* InARP reply.  */
#define ARPOP_NAK            10 /* (ATM)ARP NAK.  */

/* ARP protocol HARDWARE identifiers. */
#define ARPHRD_ETHER         1  /* Ethernet */

/*---------------------------------------------------------------------------
                            IP Address
----------------------------------------------------------------------------*/
/* Length of output buffer for inet_ntop, plus prefix length (e.g. "/128"). */
#define ADDR_STR_LEN         ((INET_ADDRSTRLEN + INET6_ADDRSTRLEN) + 5)

/* IPv4 or IPv6 address. */
struct ipaddr {
    int address_family; /* AF_INET or AF_INET6 */
    union {
        struct in_addr v4;
        struct in6_addr v6;
        uint8_t addr[16];
    } ip; /* IP address (network order) */
#define ipaddr_v4            ip.v4
#define ipaddr_v6            ip.v6
};

#define IS_IPADDR_NONE(p)    ((p)->address_family == AF_UNSPEC)
#define IS_IPADDR_V4(p)      ((p)->address_family == AF_INET)
#define IS_IPADDR_V6(p)      ((p)->address_family == AF_INET6)
#define SET_IPADDR_V4(p)     (p)->address_family = AF_INET
#define SET_IPADDR_V6(p)     (p)->address_family = AF_INET6

#define IPADDRSZ(p) \
    (IS_IPADDR_V4((p)) ? sizeof(struct in_addr) : sizeof(struct in6_addr))

#define IPADDR_STRING_SIZE 46

static inline void ip_reset(struct ipaddr* ip) {
    memset(ip, 0, sizeof(*ip));
}

/* Fill in an ipaddr using the given family-specific struct. */
extern void ip_from_ipv4(const struct in_addr* ipv4, struct ipaddr* ip);
extern void ip_from_ipv6(const struct in6_addr* ipv6, struct ipaddr* ip);

/* Fill in the given family-specific struct using the given ipaddr. */
extern void ip_to_ipv4(const struct ipaddr* ip, struct in_addr* ipv4);
extern void ip_to_ipv6(const struct ipaddr* ip, struct in6_addr* ipv6);

/* Return the number of bytes in the on-the-wire representation of
 * addresses of the given family.
 */
extern int ipaddr_length(int address_family);

/* Return the number of bytes in sockaddr of the given family. */
extern int sockaddr_length(int address_family);

/* Return true if the two addresses are the same. */
static inline bool is_equal_ip(const struct ipaddr* a,
                               const struct ipaddr* b) {
    return ((a->address_family == b->address_family) &&
            !memcmp(&a->ip, &b->ip, ipaddr_length(a->address_family)));
}

/* Parse a human-readable IPv4 address and return it. Print an error
 * to stderr and exit if there is an error parsing the address.
 */
extern struct ipaddr ipv4_parse(const char* ip_string);

/* Parse a human-readable IPv6 address and return it. Print an error
 * to stderr and exit if there is an error parsing the address.
 */
extern struct ipaddr ipv6_parse(const char* ip_string);

/* Attempt to parse a human-readable IPv4/IPv6 address and return it. Return
 * STATUS_OK on success, or STATUS_ERR on failure (meaning the input string was
 * not actually a valid IPv4 or IPv6 address).
 */
extern int ip_parse(const char* ip_string, struct ipaddr* ip);

/* Print a human-readable representation of the given IP address in the
 * given buffer, which must be at least ADDR_STR_LEN bytes long.
 * Returns a pointer to the given buffer.
 */
extern const char* ip_to_string(const struct ipaddr* ip, char* buffer);

/* Convert a string containing a human-readable DNS name or IPv/IPv6 address
 * into an IP address struct. Return STATUS_OK on success. Upon failure (i.e.,
 * the input host string was not actually a valid DNS name, IPv4 address, or
 * IPv6 address) returns STATUS_ERR and fills in *error with a malloc-allocated
 * error message.
 */
extern int string_to_ip(const char* host, struct ipaddr* ip, char** error);

/* Create an IPv4-mapped IPv6 address. */
extern struct ipaddr ipv6_map_from_ipv4(const struct ipaddr ipv4);

/* Deconstruct an IPv4-mapped IPv6 address and fill in *ipv4 with the
 * IPv4 address that was mapped into IPv6 space. Return STATUS_OK on
 * success, or STATUS_ERR on failure (meaning the input ipv6 was not
 * actually an IPv4-mapped IPv6 address).
 */
extern int ipv6_map_to_ipv4(const struct ipaddr ipv6,
                            struct ipaddr* ipv4);

/* Fill in a sockaddr struct and socklen_t using the given IP and port.
 * The IP address may be IPv4 or IPv6.
 */
extern void ip_to_sockaddr(const struct ipaddr* ip, uint16_t port,
                           struct sockaddr* address, socklen_t* length);

/* Fill in an IP address and port by parsing a sockaddr struct and
 * socklen_t using the given IP and port. The IP address may be IPv4
 * or IPv6. Exits with an error message if the address family is other
 * than AF_INET or AF_INET6.
 */
extern void ip_from_sockaddr(const struct sockaddr* address, socklen_t length,
                             struct ipaddr* ip, uint16_t* port);

/* Return true if the address is that of a local interface. */
/* Note: this should return bool, but that doesn't compile on NetBSD. */
extern int is_ip_local(const struct ipaddr* ip);

/* Fill in the name of the device configured with the given IP, if
 * any. The dev_name buffer should be at least IFNAMSIZ bytes.
 * Return true if the IP is found on a local device.
 */
/* Note: this should return bool, but that doesn't compile on NetBSD. */
extern int get_ip_device(const struct ipaddr* ip, char* dev_name);

/* Convert dotted decimal netmask to equivalent CIDR prefix length */
extern int netmask_to_prefix(const char* netmask);

bool is_ipv4(const char* host);
bool is_ipv6(const char* host);
bool is_ipaddr(const char* host);

void generate_random_ipv4_addr(char* result, const char* base,
                               const char* netmask);

void generate_random_ipv6_addr(char* result, const char* base, int prefixlen);

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001 /* Internet address 127.0.0.1.  */
#endif

uint16_t checksum(uint8_t* buf, int len);
/*---------------------------------------------------------------------------
                            Prefix Address
----------------------------------------------------------------------------*/
/* Generic prefix structure. */
struct prefix {
    uint8_t family;
    uint16_t prefixlen;
    union {
        uint8_t prefix;
        struct in_addr prefix4;
        struct in6_addr prefix6;
    } u __attribute__((aligned(8)));
};

/* IPv4 prefix structure. */
struct prefix_ipv4 {
    uint8_t family;
    uint16_t prefixlen;
    struct in_addr prefix __attribute__((aligned(8)));
};

/* IPv6 prefix structure. */
struct prefix_ipv6 {
    uint8_t family;
    uint16_t prefixlen;
    struct in6_addr prefix __attribute__((aligned(8)));
};

/*---------------------------------------------------------------------------
                            TCP/UDP Address
----------------------------------------------------------------------------*/
/* A TCP/UDP/IP address for an endpoint. */
struct endpoint {
    ipaddr_t ip; /* IP address */
    __be16 port; /* TCP/UDP port (network order) */
};

/* The 4-tuple for a TCP/UDP/IP packet. */
struct tuple {
    struct endpoint src;
    struct endpoint dst;
};

/* Return true if the two tuples are equal. */
static inline bool is_equal_tuple(const struct tuple* a,
                                  const struct tuple* b) {
    return memcmp(a, b, sizeof(*a)) == 0;
}

// origin:  1.1.1.1:8000 -> 2.2.2.2:6000
// reverse: 2.2.2.2:6000 -> 1.1.1.1:8000
static inline void reverse_tuple(const struct tuple* src_tuple,
                                 struct tuple* dst_tuple) {
    dst_tuple->src.ip = src_tuple->dst.ip;
    dst_tuple->dst.ip = src_tuple->src.ip;
    dst_tuple->src.port = src_tuple->dst.port;
    dst_tuple->dst.port = src_tuple->src.port;
}

/*---------------------------------------------------------------------------
                            VLAN Definitions
----------------------------------------------------------------------------*/
/* VLAN Identifier */
typedef uint16_t vlanid_t;
#define VLANID_MAX                     4095

/*---------------------------------------------------------------------------
                            VXLAN Definitions
----------------------------------------------------------------------------*/

/* EVPN MH DF election algorithm */
#define EVPN_MH_DF_ALG_SERVICE_CARVING 0
#define EVPN_MH_DF_ALG_HRW             1
#define EVPN_MH_DF_ALG_PREF            2

/* preference range for DF election */
#define EVPN_MH_DF_PREF_MIN            0
#define EVPN_MH_DF_PREF_DEFAULT        32767
#define EVPN_MH_DF_PREF_MAX            65535

/* VxLAN Network Identifier - 24-bit (RFC 7348) */
typedef uint32_t vni_t;
#define VNI_MAX                        16777215 /* (2^24 - 1) */

/* Flooding mechanisms for BUM packets. */
/* Currently supported mechanisms are head-end (ingress) replication
 * (which is the default) and no flooding. Future options could be
 * using PIM-SM, PIM-Bidir etc.
 */
enum vxlan_flood_control {
    VXLAN_FLOOD_HEAD_END_REPL = 0,
    VXLAN_FLOOD_DISABLED,
    VXLAN_FLOOD_PIM_SM,
};

#ifdef __cplusplus
}
#endif
#endif
