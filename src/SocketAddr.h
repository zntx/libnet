#ifndef SocketAddr_H_H
#define SocketAddr_H_H

#include "Option.h"
#include "Result.h"
#include "IpAddr.h"

class AddrParseError{
    
};
#if 0
class SocketAddrV4{
public:
    struct sockaddr_in sa;
    static Result<SocketAddrV4,int> create(std::string ips, uint16_t port);
    static Result<SocketAddrV4> parse_ascii(Slice<uint8_t> slice);
    //这是一�?仅限夜间使用的实验�? API�?( #101035addr_parse_ascii)
    //从字节片�?解析 IPv4 套接字地址�?
    SocketAddrV4(struct sockaddr_in s);
    SocketAddrV4(struct sockaddr_in *s);
    SocketAddrV4(Ipv4Addr ip,  uint16_t port);
    SocketAddrV4(const SocketAddrV4 & addr);
    SocketAddrV4(SocketAddrV4 && addr);
    SocketAddrV4& operator =(const SocketAddrV4& addr);//赋值运算�??

    Ipv4Addr ip();
    uint16_t port();
    void set_port(uint16_t port);
    std::string to_string();

};


class SocketAddrV6{
public:
    struct sockaddr_in6 sa;  /**< IPV6 地址*/

    static  Result<SocketAddrV6,int> create(std::string ips, uint16_t port);
    SocketAddrV6(struct sockaddr_in6 s);
    SocketAddrV6(struct sockaddr_in6 *s);
    SocketAddrV6(Ipv6Addr ip,  uint16_t port);

    uint16_t port();
    IpAddr ipaddr();
    std::string to_string();

};
#endif

class SocketAddr{
public:
//    bool is_v4;
//
    union {
        struct sockaddr_storage sin;
        struct sockaddr_in sin4;
        struct sockaddr_in6 sin6;
    };

    static Result<SocketAddr> Create(std::string ips);
    static Result<SocketAddr> Create(std::string ips, uint16_t port);
    static Result<SocketAddr> Create(Slice<const char> domain, uint16_t port);
    /*
     *  [ipv6]:port
     *  *.*.*.*:port
     * */
    static Result<SocketAddr> Create( Slice<const char> domain);

    SocketAddr();
    explicit SocketAddr(struct in_addr addr, uint16_t port);
    explicit SocketAddr(struct in6_addr addr, uint16_t port);
    explicit SocketAddr(IpAddr addr, uint16_t port);
    SocketAddr(struct sockaddr_storage& addr);
    explicit SocketAddr(struct sockaddr_storage* addr);
    //SocketAddr(struct sockaddr *ai_addr );
//    explicit SocketAddr(SocketAddrV4 v4);
//    explicit SocketAddr(SocketAddrV6 v6);
    SocketAddr(const SocketAddr & addr);
    SocketAddr(SocketAddr && addr)  ;



    SocketAddr& operator=(SocketAddr&& slice) ;
    SocketAddr& operator=(const SocketAddr& slice);

    bool is_v4();
    bool is_v6();
    IpAddr ipaddr();
    uint16_t port( );
    void set_port( uint16_t port);

    std::string to_string();
};


#endif