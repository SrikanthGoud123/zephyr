/**
 * Copyright (c) 2023-2024 Marcin Niestroj
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DRIVERS_NET_NSOS_H__
#define __DRIVERS_NET_NSOS_H__

#include <stddef.h>
#include <stdint.h>

/* Protocol families. */
#define NSOS_MID_PF_UNSPEC       0          /**< Unspecified protocol family.  */
#define NSOS_MID_PF_INET         1          /**< IP protocol family version 4. */
#define NSOS_MID_PF_INET6        2          /**< IP protocol family version 6. */
#define NSOS_MID_PF_UNIX         6          /**< Unix protocol.                */
#define NSOS_MID_PF_PACKET       3          /**< Packet family.                */

/* Address families. */
#define NSOS_MID_AF_UNSPEC      NSOS_MID_PF_UNSPEC   /**< Unspecified address family.   */
#define NSOS_MID_AF_INET        NSOS_MID_PF_INET     /**< IP protocol family version 4. */
#define NSOS_MID_AF_INET6       NSOS_MID_PF_INET6    /**< IP protocol family version 6. */
#define NSOS_MID_AF_UNIX        NSOS_MID_PF_UNIX     /**< Unix protocol.                */
#define NSOS_MID_AF_PACKET      NSOS_MID_PF_PACKET   /**< Packet family.                */

/** Protocol numbers from IANA/BSD */
enum nsos_mid_net_ip_protocol {
	NSOS_MID_IPPROTO_IP = 0,            /**< IP protocol (pseudo-val for setsockopt() */
	NSOS_MID_IPPROTO_ICMP = 1,          /**< ICMP protocol   */
	NSOS_MID_IPPROTO_IGMP = 2,          /**< IGMP protocol   */
	NSOS_MID_IPPROTO_ETH_P_ALL = 3,     /**< Every packet. from linux if_ether.h  */
	NSOS_MID_IPPROTO_IPIP = 4,          /**< IPIP tunnels    */
	NSOS_MID_IPPROTO_TCP = 6,           /**< TCP protocol    */
	NSOS_MID_IPPROTO_UDP = 17,          /**< UDP protocol    */
	NSOS_MID_IPPROTO_IPV6 = 41,         /**< IPv6 protocol   */
	NSOS_MID_IPPROTO_ICMPV6 = 58,       /**< ICMPv6 protocol */
	NSOS_MID_IPPROTO_RAW = 255,         /**< RAW IP packets  */
};

/** Socket type */
enum nsos_mid_net_sock_type {
	NSOS_MID_SOCK_STREAM = 1,           /**< Stream socket type   */
	NSOS_MID_SOCK_DGRAM,                /**< Datagram socket type */
	NSOS_MID_SOCK_RAW                   /**< RAW socket type      */
};

#define NSOS_MID_MSG_PEEK 0x02
#define NSOS_MID_MSG_TRUNC 0x20
#define NSOS_MID_MSG_DONTWAIT 0x40
#define NSOS_MID_MSG_WAITALL 0x100

struct nsos_mid_sockaddr {
	uint16_t sa_family;      /* Address family */
	char     sa_data[];      /* Socket address */
};

struct nsos_mid_sockaddr_in {
	uint16_t sin_family;     /* AF_INET */
	uint16_t sin_port;       /* Port number */
	uint32_t sin_addr;       /* IPv4 address */
};

struct nsos_mid_sockaddr_in6 {
	uint16_t sin6_family;    /* AF_INET6 */
	uint16_t sin6_port;      /* Port number */
	uint8_t  sin6_addr[16];
	uint32_t sin6_scope_id;  /* Set of interfaces for a scope */
};

#define UNIX_PATH_MAX 108
struct nsos_mid_sockaddr_un {
	sa_family_t sun_family;              /* AF_UNIX */
	char        sun_path[UNIX_PATH_MAX]; /* pathname */
};

struct nsos_mid_sockaddr_ll {
	sa_family_t sll_family;   /**< Always AF_PACKET                   */
	uint16_t    sll_protocol; /**< Physical-layer protocol            */
	int         sll_ifindex;  /**< Interface number                   */
	uint16_t    sll_hatype;   /**< ARP hardware type                  */
	uint8_t     sll_pkttype;  /**< Packet type                        */
	uint8_t     sll_halen;    /**< Length of address                  */
	uint8_t     sll_addr[8];  /**< Physical-layer address, big endian */
};

struct nsos_mid_sockaddr_storage {
	union {
		struct nsos_mid_sockaddr_in sockaddr_in;
		struct nsos_mid_sockaddr_in6 sockaddr_in6;
		struct nsos_mid_sockaddr_un sockaddr_un;
		struct nsos_mid_sockaddr_ll sockaddr_ll;
	};
};

struct nsos_mid_pollfd {
	int fd;
	short events;
	short revents;

	void (*cb)(struct nsos_mid_pollfd *pollfd_mid);
};

struct nsos_mid_addrinfo {
	int                       ai_flags;
	int                       ai_family;
	int                       ai_socktype;
	int                       ai_protocol;
	size_t                    ai_addrlen;
	struct nsos_mid_sockaddr *ai_addr;
	char                     *ai_canonname;
	struct nsos_mid_addrinfo *ai_next;
};

struct nsos_mid_iovec {
	void  *iov_base;
	size_t iov_len;
};

struct nsos_mid_msghdr {
	void                  *msg_name;       /* optional socket address, big endian */
	size_t                 msg_namelen;    /* size of socket address */
	struct nsos_mid_iovec *msg_iov;        /* scatter/gather array */
	size_t                 msg_iovlen;     /* number of elements in msg_iov */
	void                  *msg_control;    /* ancillary data */
	size_t                 msg_controllen; /* ancillary data buffer len */
	int                    msg_flags;      /* flags on received message */
};

static inline void nsos_socket_flag_convert(int *flags_a, int flag_a,
					    int *flags_b, int flag_b)
{
	if ((*flags_a) & flag_a) {
		*flags_a &= ~flag_a;
		*flags_b |= flag_b;
	}
}

int nsos_adapt_get_errno(void);

int nsos_adapt_socket(int family, int type, int proto);

int nsos_adapt_bind(int fd, const struct nsos_mid_sockaddr *addr, size_t addrlen);
int nsos_adapt_connect(int fd, const struct nsos_mid_sockaddr *addr, size_t addrlen);
int nsos_adapt_listen(int fd, int backlog);
int nsos_adapt_accept(int fd, struct nsos_mid_sockaddr *addr, size_t *addrlen);
int nsos_adapt_sendto(int fd, const void *buf, size_t len, int flags,
		      const struct nsos_mid_sockaddr *addr, size_t addrlen);
int nsos_adapt_sendmsg(int fd, const struct nsos_mid_msghdr *msg_mid, int flags);
int nsos_adapt_recvfrom(int fd, void *buf, size_t len, int flags,
			struct nsos_mid_sockaddr *addr, size_t *addrlen);
int nsos_adapt_getsockopt(int fd, int level, int optname,
			  void *optval, size_t *optlen);
int nsos_adapt_setsockopt(int fd, int level, int optname,
			  const void *optval, size_t optlen);

void nsos_adapt_poll_add(struct nsos_mid_pollfd *pollfd);
void nsos_adapt_poll_remove(struct nsos_mid_pollfd *pollfd);
void nsos_adapt_poll_update(struct nsos_mid_pollfd *pollfd);

int nsos_adapt_fcntl_getfl(int fd);
int nsos_adapt_fcntl_setfl(int fd, int flags);

int nsos_adapt_fionread(int fd, int *avail);

int nsos_adapt_dup(int oldfd);

int nsos_adapt_getaddrinfo(const char *node, const char *service,
			   const struct nsos_mid_addrinfo *hints,
			   struct nsos_mid_addrinfo **res,
			   int *system_errno);
void nsos_adapt_freeaddrinfo(struct nsos_mid_addrinfo *res);

#endif /* __DRIVERS_NET_NSOS_H__ */
