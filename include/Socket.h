#ifndef _SOCKETS_Socket_H
#define _SOCKETS_Socket_H

#include <string>
#include <vector>
#include <list>
#include "socket_include.h"

#include <time.h>
#include "IpAddr.h"
#include "SocketAddr.h"
#include "Slice.h"


enum class Direction {
    Read,
    Write,
    All
};


/** \defgroup basic Basic sockets */
/** Socket base class.
	\ingroup basic */
class Socket
{
    /** Socket mode flags. */
/*
	enum {
		// Socket
		SOCK_DEL = 			0x01, ///< Delete by handler flag
		SOCK_CLOSE = 			0x02, ///< Close and delete flag
		SOCK_DISABLE_READ = 		0x04, ///< Disable checking for read events
		SOCK_CONNECTED = 		0x08, ///< Socket is connected (tcp/udp)

		SOCK_ERASED_BY_HANDLER = 	0x10, ///< Set by handler before delete
		// HAVE_OPENSSL
		SOCK_ENABLE_SSL = 		0x20, ///< Enable SSL for this TcpSocket
		SOCK_SSL = 			0x40, ///< ssl negotiation mode (TcpSocket)
		SOCK_SSL_SERVER = 		0x80, ///< True if this is an incoming ssl TcpSocket connection

		// ENABLE_IPV6
		SOCK_IPV6 = 			0x0100, ///< This is an ipv6 socket if this one is true
		// ENABLE_POOL
		SOCK_CLIENT = 			0x0200, ///< only client connections are pooled
		SOCK_RETAIN = 			0x0400, ///< keep connection on close
		SOCK_LOST = 			0x0800, ///< connection lost

		// ENABLE_SOCKS4
		SOCK_SOCKS4 = 			0x1000, ///< socks4 negotiation mode (TcpSocket)
		// ENABLE_DETACH
		SOCK_DETACH = 			0x2000, ///< Socket ordered to detach flag
		SOCK_DETACHED = 		0x4000, ///< Socket has been detached
		// StreamSocket
		STREAMSOCK_CONNECTING =		0x8000, ///< Flag indicating connection in progress

		STREAMSOCK_FLUSH_BEFORE_CLOSE = 0x010000L, ///< Send all data before closing (default true)
		STREAMSOCK_CALL_ON_CONNECT =	0x020000L, ///< OnConnect will be called next ISocketHandler cycle if true
		STREAMSOCK_RETRY_CONNECT =	0x040000L, ///< Try another connection attempt next ISocketHandler cycle
		STREAMSOCK_LINE_PROTOCOL =	0x080000L, ///< Line protocol mode flag

	};
*/

public:
    /** "Default" constructor */
    Socket( SOCKET _soket = 0 );
    Socket( Socket&&  soc);

    virtual ~Socket();

    static Socket * Create(int famliy, int type, int protno);

    /** Create a socket file descriptor.
        \param af Address family AF_INET / AF_INET6 / ...
        \param type SOCK_STREAM / SOCK_DGRAM / ...
        \param protocol "tcp" / "udp" / ... */
    SOCKET Create(int af, int type, const std::string& protocol = "");

    bool connect(SocketAddr addr);

    bool bind(SocketAddr& addr);

    bool Select(Direction direction, uint32_t msecond);

    /** Return file descriptor assigned to this socket. */
    SOCKET get_socket();

    SOCKET take();

    SocketAddr * local();
    /** Returns local port number for bound socket file descriptor. */
    uint16_t local_port();

    IpAddr * local_ipaddr();

    SocketAddr * peer();
    /** Returns remote port number: ipv4 and ipv6. */
    uint16_t peer_port();

    /** Get address/port of last connect() call. */
    IpAddr * peer_addr();

    /** get socket non-block operation. */
    bool no_blocking();
    
    /** Set socket non-block operation. */
    bool set_no_blocking(bool);

    bool nodelay();
    /** Set socket non-block operation. */
    bool set_nodelay(bool bNb);

    /** \name Event callbacks */
    //@{

    /** Called when a client socket is created, to set socket options.
        \param family AF_INET, AF_INET6, etc
        \param type SOCK_STREAM, SOCK_DGRAM, etc
        \param protocol Protocol number (tcp, udp, sctp, etc)
        \param s Socket file descriptor
    */
    //virtual void OnOptions(int family,int type,int protocol,SOCKET s) = 0;
    /** Connection retry callback - return false to abort connection attempts */
    //virtual bool OnConnectRetry();
#ifdef ENABLE_RECONNECT
    /** a reconnect has been made */
	virtual void OnReconnect();
#endif
    /** TcpSocket: When a disconnect has been detected (recv/SSL_read returns 0 bytes). */
    //virtual void OnDisconnect();
    /** TcpSocket: When a disconnect has been detected (recv/SSL_read returns 0 bytes).
        \param info bit 0 read(0)/write(1)
                    bit 1 normal(read or write returned 0)/error(r/w returned -1)
                    bit 2 ssl
        \param code error code from read/write call (errno / ssl error)
    */
    //virtual void OnDisconnect(short info, int code);
    /** Timeout callback. */
    //virtual void OnTimeout();
    /** Connection timeout. */
    //virtual void OnConnectTimeout();
    //@}

    // --------------------------------------------------------------------------
    /** @name IP options
       When an ip or socket option is available on all of the operating systems
       I'm testing on (linux 2.4.x, _win32, macosx, solaris9 intel) they are not
       checked with an #ifdef below.
       This might cause a compile error on other operating systems. */
    // --------------------------------------------------------------------------

    // IP options
    //@{

    bool SetIpOptions(const void *p, socklen_t len);
    bool SetIpTOS(unsigned char tos);
    unsigned char IpTOS();
    bool SetIpTTL(int ttl);
    int IpTTL();
    bool SetIpHdrincl(bool x = true);
    bool SetIpMulticastTTL(int);
    int IpMulticastTTL();
    bool SetMulticastLoop(bool x = true);
    bool IpAddMembership(struct ip_mreq&);
    bool IpDropMembership(struct ip_mreq&);

#ifdef IP_PKTINFO
    bool SetIpPktinfo(bool x = true);
#endif
#ifdef IP_RECVTOS
    bool SetIpRecvTOS(bool x = true);
#endif
#ifdef IP_RECVTTL
    bool SetIpRecvTTL(bool x = true);
#endif
#ifdef IP_RECVOPTS
    bool SetIpRecvopts(bool x = true);
#endif
#ifdef IP_RETOPTS
    bool SetIpRetopts(bool x = true);
#endif
#ifdef IP_RECVERR
    bool SetIpRecverr(bool x = true);
#endif
#ifdef IP_MTU_DISCOVER
    bool SetIpMtudiscover(bool x = true);
#endif
#ifdef IP_MTU
    int IpMtu();
#endif
#ifdef IP_ROUTER_ALERT
    bool SetIpRouterAlert(bool x = true);
#endif
#ifdef LINUX
    bool IpAddMembership(struct ip_mreqn&);
#endif
#ifdef LINUX
    bool IpDropMembership(struct ip_mreqn&);
#endif
    //@}

    // SOCKET options
    /** @name Socket Options */
    //@{

    bool SoAcceptconn();
    bool SetSoBroadcast(bool x = true);
    bool SetSoDebug(bool x = true);
    int SoError();
    bool SetSoDontroute(bool x = true);
    bool set_close_delay(int onoff, int linger);
    bool SetSoOobinline(bool x = true);
    bool SetSoRcvlowat(int);
    bool SetSoSndlowat(int);
    bool SetSoRcvtimeo(struct timeval&);

    bool set_write_timeout(struct timeval &tv);
    timeval * write_timeout( );

    bool set_read_timeout(struct timeval& tv);
    timeval * read_timeout( );

    bool SetSoRcvbuf(int);
    int SoRcvbuf();
    bool SetSoSndbuf(int);
    int SoSndbuf();
    int SoType();
    bool set_reuse(bool x = true);
    bool SetSoKeepalive(bool x = true);

#ifdef SO_BSDCOMPAT
    bool SetSoBsdcompat(bool x = true);
#endif
#ifdef SO_BINDTODEVICE
    bool SetSoBindtodevice(const std::string& intf);
#endif
#ifdef SO_PASSCRED
    bool SetSoPasscred(bool x = true);
#endif
#ifdef SO_PEERCRED
    bool SoPeercred(struct ucred& );
#endif
#ifdef SO_PRIORITY
    bool SetSoPriority(int);
#endif
#ifdef SO_RCVBUFFORCE
    bool SetSoRcvbufforce(int);
#endif
#ifdef SO_SNDBUFFORCE
    bool SetSoSndbufforce(int);
#endif
#ifdef SO_TIMESTAMP
    bool SetSoTimestamp(bool x = true);
#endif
#ifdef SO_NOSIGPIPE
    bool SetSoNosigpipe(bool x = true);
#endif
    //@}

    // TCP options in TcpSocket.h/TcpSocket.cpp


    // LIST_CALLONCONNECT

    /** Instruct socket to call OnConnect callback next sockethandler cycle. */
    void SetCallOnConnect(bool x = true);

    /** Check call on connect flag.
        \return true if OnConnect() should be called a.s.a.p */
    bool CallOnConnect();

    // LIST_RETRY

    /** Set flag to initiate a connection attempt after a connection timeout. */
    void SetRetryClientConnect(bool x = true);

    /** Check if a connection attempt should be made.
        \return true when another attempt should be made */
    bool RetryClientConnect();

    /** Enable ipv6 for this socket. */
	void SetIpv6(bool x = true);
	/** Check ipv6 socket.
		\return true if this is an ipv6 socket */
	bool IsIpv6();

protected:
    /** assignment operator not available. */
    Socket& operator=(const Socket& ) { return *this; }

    SOCKET fd; ///< File descriptor

    bool m_ipv6; ///< This is an ipv6 socket if this one is true

#ifdef _WIN32
    static	WSAInitializer m_winsock_init; ///< Winsock initialization singleton class
#endif
};


#endif // _SOCKETS_Socket_H

