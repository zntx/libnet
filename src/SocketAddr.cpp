#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "IpAddr.h"
#include "SocketAddr.h"

#if 0
Result<SocketAddrV4> SocketAddrV4::parse_ascii(Slice<uint8_t> slice)
{
    return Err(std::string(StrError(Errno)));
}

SocketAddrV4::SocketAddrV4(Ipv4Addr ip, uint16_t port)
{
    sa.sin_addr.s_addr = ip.sin.s_addr;
    sa.sin_port = htons(port);
}

SocketAddrV4::SocketAddrV4(struct sockaddr_in s)
{
    memcpy(&sa, &s, sizeof(struct sockaddr_in));
}

SocketAddrV4::SocketAddrV4(struct sockaddr_in *s)
{
    memcpy(&sa, s, sizeof(struct sockaddr_in));
}

SocketAddrV4::SocketAddrV4(const SocketAddrV4 &addr)
{
    memcpy(&sa, &addr.sa, sizeof(struct sockaddr_in));
}

SocketAddrV4::SocketAddrV4(SocketAddrV4 &&addr)
{
    memcpy(&sa, &addr.sa, sizeof(struct sockaddr_in));
}
SocketAddrV4 &SocketAddrV4::operator=(const SocketAddrV4 &addr) // 赋值运算�??
{
    memcpy(&sa, &addr.sa, sizeof(struct sockaddr_in));
    return *this;
}

Ipv4Addr SocketAddrV4::ip()
{
    return Ipv4Addr(this->sa.sin_addr);
}
uint16_t SocketAddrV4::port()
{
    return ntohs(this->sa.sin_port);
}
void SocketAddrV4::set_port(uint16_t port)
{
    this->sa.sin_port = htons(port);
}

std::string SocketAddrV4::to_string()
{
    return std::string(inet_ntoa(this->sa.sin_addr)) + ":" + std::to_string(this->port());
}

/*

*/
SocketAddrV6::SocketAddrV6(Ipv6Addr ip, uint16_t port)
{
    // ss.sin_addr.s_addr = inet_addr(ip.ips);
    // inet_pton_with_af(AF_INET6, ip.ips, &(ss.sin6_addr), sizeof(struct in6_addr));
    sa.sin6_port = htons(port);
    sa.sin6_family = AF_INET6;
}

SocketAddrV6::SocketAddrV6(struct sockaddr_in6 s)
{
    memcpy(&sa, &s, sizeof(struct sockaddr_in6));
}
SocketAddrV6::SocketAddrV6(struct sockaddr_in6 *s)
{
    memcpy(&sa, s, sizeof(struct sockaddr_in6));
}

uint16_t SocketAddrV6::port()
{
    return ntohs(this->sa.sin6_port);
}

std::string SocketAddrV6::to_string()
{
    char ip_str[INET6_ADDRSTRLEN];

    const void *src = &(this->sa.sin6_addr);
    const char *str = inet_ntop(AF_INET6, src, ip_str, INET6_ADDRSTRLEN); // 将数值格式转化为点分十进制的ip地址格式
    if (str == NULL)
    {
    }

    return std::string(ip_str) + ":" + std::to_string(this->port());
}
#endif
/*

*/
Result<SocketAddr> SocketAddr::Create(std::string ips)
{
    return Create( Slice<const char>(ips.c_str(), ips.length()));
}

Result<SocketAddr> SocketAddr::Create(std::string ips, uint16_t port)
{
    return Create( Slice<const char>(ips.c_str(), ips.length()), port);
}

Result<SocketAddr> SocketAddr::Create(Slice<const char> domain, uint16_t port)
{
    // 判断IP 是V4还是V6
    auto r_addr = IpAddr::Create(domain);
    if (r_addr.is_err())
    {
        return Err(r_addr.unwrap_err());
    }

    IpAddr addr = r_addr.unwrap();

    if (addr.is_v4)
    {
        return Ok(SocketAddr(addr.sin4, port));
    }

    return Ok(SocketAddr(addr.sin6, port));
}

Result<SocketAddr> SocketAddr::Create( Slice<const char> domain)
{
    auto start = domain.find('[');
    if( !start.is_empty()) // [:::::]:port
    {
        const char* end_str = "]:";
        auto end = domain.find(Slice<const char>(end_str, 2));
        if( start.is_empty())
        {
            return Err(std::string("[]: 格式不对"));
        }

        auto start_real = start.unwrap();
        auto end_real = end.unwrap();

        auto ips = domain.slice(start_real + 1, end_real);
        auto ports = domain.slice(end_real + 3);

        if( ips.is_empty() || ports.is_empty())
        {
            return Err(std::string("[]: 格式不对"));
        }

        int port = atoi(ports.unwrap().addr);

        return Create(domain, (uint16_t)port);
    }

    if(domain.find_count(':') > 1)//ipcv6
    {
        printf("find ip v6\n");
        return Create(domain, 0);
    }

    auto postion = domain.find(':');
    if(postion.is_empty())
    {
        return Create(domain, 0);
    }
    else //*.*.*.*:port
    {
        auto postion_real = postion.unwrap();
        auto ips = domain.slice(0, postion_real);
        auto ports = domain.slice(postion_real + 1);

        if( ips.is_empty() || ports.is_empty())
        {
            return Err(std::string("[]: 格式不对"));
        }

        int port = atoi(ports.unwrap().addr);

        auto ips_slic = ips.unwrap();
        printf("ips_slic %s %zu\n",  ips_slic.addr, ips_slic.len );

        return Create(ips_slic, (uint16_t)port);
    }

}

SocketAddr::SocketAddr()
{
    sin.ss_family = AF_UNSPEC;
}

SocketAddr::SocketAddr(struct in_addr addr, uint16_t port)
{
    sin.ss_family = AF_INET;
    sin4.sin_addr = addr;
    sin4.sin_port = htons(port);
}

SocketAddr::SocketAddr(struct in6_addr addr, uint16_t port)
{
    sin.ss_family = AF_INET6;
    sin6.sin6_addr = addr;
    sin6.sin6_port = htons(port);
}

SocketAddr::SocketAddr(IpAddr addr, uint16_t port)
{
    if(addr.is_ipv4( ) ){
        sin4.sin_family = AF_INET;
        sin4.sin_addr = addr.sin4.sin;
        sin4.sin_port = htons(port);
    }else{
        sin6.sin6_family = AF_INET;
        //sin6.sin6_addr = addr.sin6.sin;
        sin6.sin6_port = htons(port);
    }
}

SocketAddr::SocketAddr(struct sockaddr_storage& addr)
{
    sin.ss_family = addr.ss_family;

    if (sin.ss_family == AF_INET )
        memcpy(&sin4, &addr, sizeof(struct sockaddr_in));
    else if( sin.ss_family == AF_INET6 )
        memcpy(&sin4, &addr, sizeof(struct sockaddr_in6));

}

//SocketAddr::SocketAddr(struct sockaddr_storage &addr)
//{
//    sin.ss_family = addr.ss_family;
//
//    if (sin.ss_family == AF_INET )
//        memcpy(&sin4, &addr, sizeof(struct sockaddr_in));
//    else if( sin.ss_family == AF_INET6 )
//        memcpy(&sin4, &addr, sizeof(struct sockaddr_in6));
//
//}

SocketAddr::SocketAddr(struct sockaddr_storage *addr)
{
    sin.ss_family = addr->ss_family;

    if (sin.ss_family == AF_INET )
        memcpy(&sin4, addr, sizeof(struct sockaddr_in));
    else if( sin.ss_family == AF_INET6 )
        memcpy(&sin4, addr, sizeof(struct sockaddr_in6));

}

//SocketAddr::SocketAddr(SocketAddrV4 v4)
//{
//    memcpy(&sin4, &v4.sa, sizeof(struct sockaddr_in));
//}
//SocketAddr::SocketAddr(SocketAddrV6 v6)
//{
//    memcpy(&sin6, &v6.sa, sizeof(struct sockaddr_in6));
//}

SocketAddr::SocketAddr(const SocketAddr &addr)
{
    if (addr.sin.ss_family == AF_INET)
    {
        memcpy(&sin4, &addr.sin4, sizeof(struct sockaddr_in));
    }
    else
    {
        memcpy(&sin6, &addr.sin6, sizeof(struct sockaddr_in6));
    }
}
SocketAddr::SocketAddr(SocketAddr &&addr)
{
    if (addr.sin.ss_family == AF_INET)
    {
        memcpy(&this->sin4, &addr.sin4, sizeof(struct sockaddr_in));
    }
    else
    {
        memcpy(&this->sin6, &addr.sin6, sizeof(struct sockaddr_in6));
    }
}

SocketAddr &SocketAddr::operator=(SocketAddr &&addr)
{
    if (addr.sin.ss_family == AF_INET)
    {
        memcpy(&sin4, &addr.sin4, sizeof(struct sockaddr_in));
    }
    else
    {
        memcpy(&sin6, &addr.sin6, sizeof(struct sockaddr_in6));
    }
    return *this;
}
SocketAddr &SocketAddr::operator=(const SocketAddr &addr)
{
    if (addr.sin.ss_family == AF_INET)
    {
        memcpy(&sin4, &addr.sin4, sizeof(struct sockaddr_in));
    }
    else
    {
        memcpy(&sin6, &addr.sin6, sizeof(struct sockaddr_in6));
    }

    return *this;
}

IpAddr  SocketAddr::ipaddr()
{
    if ( sin.ss_family == AF_INET)
    {
        return IpAddr(this->sin4.sin_addr);
    }
    else
    {
        return IpAddr(this->sin6.sin6_addr);
    }
}

uint16_t SocketAddr::port( )
{
    if ( sin.ss_family == AF_INET)
    {
        return ntohs(this->sin4.sin_port);
    }
    else
    {
        return ntohs(this->sin6.sin6_port);
    }
}

void SocketAddr::set_port( uint16_t port)
{
    if ( sin.ss_family == AF_INET)
    {
        this->sin4.sin_port = htons(port);
    }
    else
    {
        this->sin6.sin6_port= htons(port);
    }
}

std::string SocketAddr::to_string()
{
    char ip_str[INET6_ADDRSTRLEN] = {0};
    if ( sin.ss_family == AF_INET)
    {
        const void *src = &(this->sin4.sin_addr);
        const char *str = inet_ntop(AF_INET, src, ip_str, INET6_ADDRSTRLEN); // 将数值格式转化为点分十进制的ip地址格式
        if (str == NULL)
        {
        }

        return std::string(ip_str) + ":" + std::to_string(this->port());
    }
    else
    {
        const void *src = &(this->sin6.sin6_addr);
        const char *str = inet_ntop(AF_INET6, src, ip_str, INET6_ADDRSTRLEN); // 将数值格式转化为点分十进制的ip地址格式
        if (str == NULL)
        {
        }

        return std::string(ip_str) + ":" + std::to_string(this->port());
    }
}

bool SocketAddr::is_v4()
{
    return  sin.ss_family == AF_INET;
}

bool SocketAddr::is_v6()
{
    return  sin.ss_family == AF_INET6;
}


