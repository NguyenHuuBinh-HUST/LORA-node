#ifndef MDM_SOCKET_HAL_H
#define MDM_SOCKET_HAL_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct _sockaddr_t
{
    uint16_t   sa_family;
    uint8_t    sa_data[14];
} sockaddr_t;

typedef uint32_t sock_handle_t;
typedef uint32_t socklen_t;
typedef int32_t sock_result_t;

static const uint8_t SOCKET_STATUS_INACTIVE = 1;
static const uint8_t SOCKET_STATUS_ACTIVE = 0;

//--------- Address Families --------
#define  AF_INET                2
#define  AF_INET6               23

#define NIF_DEFAULT 0

enum hal_socket_type
{
	SOCK_STREAM=1,
	SOCK_DGRAM=2,

};

//----------- Socket Protocol ----------
#define IPPROTO_IP              0           // dummy for IP
#define IPPROTO_ICMP            1           // control message protocol
#define IPPROTO_IPV4            IPPROTO_IP  // IP inside IP
#define IPPROTO_TCP             6           // tcp
#define IPPROTO_UDP             17          // user datagram protocol
#define IPPROTO_IPV6            41          // IPv6 in IPv6
#define IPPROTO_NONE            59          // No next header
#define IPPROTO_RAW             255         // raw IP packet
#define IPPROTO_MAX             256

//----------- Socket Functions ---------
sock_handle_t socket_create(uint8_t family, uint8_t type, uint8_t protocol, uint16_t port, uint8_t nif);

sock_result_t socket_connect(sock_handle_t sd, const sockaddr_t *addr, long addrlen);

sock_result_t socket_receive(sock_handle_t sd, void* buffer, socklen_t len, uint32_t _timeout);

sock_result_t socket_receivefrom(sock_handle_t sd, void* buffer, socklen_t len, uint32_t flags, sockaddr_t* address, socklen_t* addr_size);

sock_result_t socket_send(sock_handle_t sd, const void* buffer, socklen_t len);

sock_result_t socket_sendto(sock_handle_t sd, const void* buffer, socklen_t len, uint32_t flags, sockaddr_t* addr, socklen_t addr_size);

sock_result_t socket_close(sock_handle_t sd);

uint8_t socket_active_status(sock_handle_t socket);

uint8_t socket_handle_valid(sock_handle_t handle);

sock_handle_t socket_handle_invalid();

#ifdef	__cplusplus
}
#endif
#endif
