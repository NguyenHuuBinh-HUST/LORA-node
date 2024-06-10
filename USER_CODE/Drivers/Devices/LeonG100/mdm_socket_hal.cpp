#include "mdm_socket_hal.h"
#include "mdm_hal.h"

const sock_handle_t SOCKET_MAX = (sock_handle_t)7; // 7 total sockets, handle 0-6
const sock_handle_t SOCKET_INVALID = (sock_handle_t)-1;


sock_handle_t socket_create(uint8_t family, uint8_t type, uint8_t protocol, uint16_t port, uint8_t nif)
{
    sock_handle_t handle = thanhntMDM.socketSocket(protocol==IPPROTO_TCP ? MDM_IPPROTO_TCP : MDM_IPPROTO_UDP, port);
    thanhntMDM.socketSetBlocking(handle, 0);
    return handle;
}

sock_result_t socket_connect(sock_handle_t sd, const sockaddr_t *addr, long addrlen)
{
    const uint8_t* addr_data = addr->sa_data;
    uint16_t port = addr_data[0]<<8 | addr_data[1];
    MDM_IP ip = IPADR(addr_data[2], addr_data[3], addr_data[4], addr_data[5]);
    thanhntMDM.socketSetBlocking(sd, 5000);
    bool result = thanhntMDM.socketConnect(sd, ip, port);
    thanhntMDM.socketSetBlocking(sd, 0);
    return (result ? 0 : 1);
}

sock_result_t socket_receive(sock_handle_t sd, void* buffer, socklen_t len, uint32_t _timeout)
{
    thanhntMDM.socketSetBlocking(sd, _timeout);
    sock_result_t result = 0;
    if (_timeout==0) {
    		result = thanhntMDM.socketReadable(sd);
    		if (result==0)		// no data, so return without polling for data
    			return 0;
    		if (result>0)		// clear error
    			result = 0;
    }
    if (!result)
    		result = thanhntMDM.socketRecv(sd, (char*)buffer, len);
    return result;
}

sock_result_t socket_receivefrom(sock_handle_t sock, void* buffer, socklen_t bufLen, uint32_t flags, sockaddr_t* addr, socklen_t* addrsize)
{
    int port;
    MDM_IP ip;

    sock_result_t result = thanhntMDM.socketReadable(sock);
	if (result<=0)			// error or no data
		return result;

	// have some data to let's get it.
	result = thanhntMDM.socketRecvFrom(sock, &ip, &port, (char*)buffer, bufLen);
    if (result > 0) {
        uint32_t ipv4 = ip;
        addr->sa_data[0] = (port>>8) & 0xFF;
        addr->sa_data[1] = port & 0xFF;
        addr->sa_data[2] = (ipv4 >> 24) & 0xFF;
        addr->sa_data[3] = (ipv4 >> 16) & 0xFF;
        addr->sa_data[4] = (ipv4 >> 8) & 0xFF;
        addr->sa_data[5] = ipv4 & 0xFF;
    }
    return result;
}

sock_result_t socket_send(sock_handle_t sd, const void* buffer, socklen_t len)
{
    return thanhntMDM.socketSend(sd, (const char*)buffer, len);
}

sock_result_t socket_sendto(sock_handle_t sd, const void* buffer, socklen_t len, uint32_t flags, sockaddr_t* addr, socklen_t addr_size)
{
    const uint8_t* addr_data = addr->sa_data;
    uint16_t port = addr_data[0]<<8 | addr_data[1];
    MDM_IP ip = IPADR(addr_data[2], addr_data[3], addr_data[4], addr_data[5]);
    return thanhntMDM.socketSendTo(sd, ip, port, (const char*)buffer, len);
}

sock_result_t socket_close(sock_handle_t sock)
{
    bool result = thanhntMDM.socketFree(sock); // closes and frees the socket
    return (result ? 0 : 1);
}

uint8_t socket_active_status(sock_handle_t socket)
{
    bool result = thanhntMDM.socketIsConnected(socket);
    return (result ? 0 : 1);
}

uint8_t socket_handle_valid(sock_handle_t handle)
{
    return (handle<SOCKET_MAX ? 1 : 0);
}

sock_handle_t socket_handle_invalid()
{
    return SOCKET_INVALID;
}
