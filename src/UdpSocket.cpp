#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <algorithm>
#include <utility>
#include "../include/UdpSocket.h"



UdpSocket * UdpSocket::Bind(SocketAddr host , int backlog)
{
    auto _server = Socket::Create(host.sin4.sin_family, SOCK_STREAM, IPPROTO_TCP);
    if( _server == nullptr)
    {
        printf("Socket::Create error\n");
        return nullptr;
    }

    if(!_server->bind(host) )
        return nullptr;

    if(listen(_server->get_socket(), backlog) != 0)
    {
        return nullptr;
    }

    return new(std::nothrow) UdpSocket(_server->take());
}


UdpSocket * UdpSocket::Bind(Slice<const char> host, uint16_t port, int backlog)
{
    SOCKET stream = INVALID_SOCKET;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_flags =  AI_ALL; //AI_NUMERICHOST; //
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;
    struct addrinfo *res = nullptr;
    //if (Utility::isipv6(host))
    //    hints.ai_flags |= AI_NUMERICHOST;

    std::cout << "Connect host :" << host.to_string() << "."<<std::endl;
    int status = getaddrinfo(host.to_string().c_str(), nullptr, &hints, &res);
    if (! status )
    {
        for (struct addrinfo *p = res; p != NULL; p = p->ai_next)
        {
            auto ip_addr =  SocketAddr((struct sockaddr_storage*)p->ai_addr);

            ip_addr.set_port(port);

            std::cout << "ip_addr :" << ip_addr.to_string() << "." << std::endl;

            auto server = Bind(ip_addr);
            if( server == nullptr)
                continue;

            stream = server->take();
            std::cout << "find "<<std::endl;
            break;
        }

        freeaddrinfo(res);
    }
    else
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return nullptr;
    }

    if (stream != INVALID_SOCKET)
    {
        if (::listen(stream, 24) != 0)
        {
            fprintf(stdout, "%s:%d socket_listen failed: errno %d\n", __FILE__, __LINE__, errno);
            close(stream);
            return nullptr;
        }

        return new(std::nothrow) UdpSocket(stream);
    }

    return nullptr;
}


UdpSocket * UdpSocket::Bind(std::string domain, uint16_t port, int backlog) {
    return Bind(Slice<const char>{domain.c_str(), domain.length()}, port, 0);
}

UdpSocket * UdpSocket::Bind(std::string domain, int backlog) {

    int num = std::count(domain.begin(),domain.end(),':');
    std::cout << " std::count " << num << std::endl;
    if( num >= 2)
    {
        std::size_t start = domain.find('[');
        std::size_t end = domain.find("]:");

        if( start == std::string::npos && end == std::string::npos)
        {
            return Bind(Slice<const char>(domain.c_str(), domain.length()), 80, 0);
        }

        if ( start ==  std::string::npos || end == std::string::npos)
        {
            return nullptr;
        }

        if (  start > end )
        {
            return nullptr;
        }
        std::string host = domain.substr(start + 1, end);   // get from "live" to the end

        std::string ports = domain.substr(end +2); // get from "live" to the end
        if( ports.empty())
            return nullptr;

        int port = atoi(ports.c_str());

        return Bind(Slice<const char>(host.c_str(), host.length()), port, 0);
    }
    else if (num == 1)
    {
        std::size_t pos = domain.find(':'); // position of "live" in str
        std::string host = domain.substr(0, pos);   // get from "live" to the end
        std::string ports = domain.substr(pos + 1); // get from "live" to the end
        int port = atoi(ports.c_str());

        return Bind(Slice<const char>(host.c_str(), host.length()), port, 0);
    }
    else
    {
        return Bind(Slice<const char>(domain.c_str(), domain.length()), 80, 0);
    }
}



















UdpSocket * UdpSocket::Connect(SocketAddr host, struct timeval timeout)
{
    int family = host.is_v4() ? AF_INET : AF_INET6;

    auto client = Socket::Create(family, SOCK_STREAM, IPPROTO_TCP);
    if( !client )
    {
        printf("Socket::Create error\n");
        return nullptr;
    }

    /* 没有超时时间*/
    if( timeout.tv_sec == 0 && timeout.tv_usec == 0)
    {
        auto ret = client->connect(host);

        if( !ret )
        {
            return nullptr;
        }

        return new(std::nothrow) UdpSocket(client->take());
    }

    // 清除错误
    int opt_val = 0;
    int length = sizeof(opt_val);
    auto socket_fd = client->get_socket( );

    client->set_nodelay(1);

    printf("time start %lld\n", time(nullptr));
    auto ret = client->connect(host);
    if( !ret )
    {
#ifdef  __WINDOWS__
        if( Errno == WSAEWOULDBLOCK)
#else
            if( Errno == EINPROGRESS)
#endif
        {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(socket_fd, &set);

            if(::select(socket_fd + 1, nullptr, &set, nullptr, &timeout) > 0)
            {
                getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, (char*)&opt_val, (socklen_t*)&length);
            }
        }
    }
    printf("time end %lld\n", time(nullptr));
    client->set_nodelay(0);

    if (0 == opt_val)
    {
        return new(std::nothrow) UdpSocket(client->take());
    }
    else{
        return nullptr;
    }
}

UdpSocket * UdpSocket::Connect(Slice<const char> host, size_t port, struct timeval timeout)
{
    auto stream = INVALID_SOCKET;
    struct addrinfo hints{0};
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags =  AI_ALL;       //AI_NUMERICHOST; //
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;
    struct addrinfo *res = nullptr;
    //if (Utility::isipv6(host))
    //    hints.ai_flags |= AI_NUMERICHOST;

    std::cout << "Connect host :" << host.to_string() << "."<<std::endl;
    int n = getaddrinfo(host.to_string().c_str(), nullptr, &hints, &res);
    if (!n)
    {
        for (struct addrinfo *ai = res ; ai != nullptr; ai = ai -> ai_next)
        {

            auto ip_addr =  SocketAddr((struct sockaddr_storage*)ai->ai_addr);

            ip_addr.set_port(port);

            std::cout << "ip_addr :" << ip_addr.to_string() << "." << std::endl;

            auto client = Connect(ip_addr, timeout);
            if( !client)
                continue;

            stream = client->take();
            std::cout << "find "<<std::endl;
            break;

        }
        freeaddrinfo(res);
    }
    else
    {
        std::cout << "getaddrinfo error :" <<std::endl;
        auto ips =  SocketAddr::Create( host, port);
        if( !ips ){
            return nullptr;
        }

        ips->set_port(port);

        std::cout << "ips :" << ips->to_string() << "."<<std::endl;

        auto client = Connect(ips, timeout);
        if( !client )
            return nullptr;

        stream = client.unwrap().take();

    }

    if( stream == INVALID_SOCKET)
        return nullptr;

    return new(std::nothrow) UdpSocket(stream);
}

UdpSocket * UdpSocket::Connect(const std::string& domain, struct timeval timeout )
{
    int num = std::count(domain.begin(),domain.end(),':');
    std::cout << " std::count " << num << std::endl;
    if( num >= 2)
    {
        std::size_t start = domain.find('[');
        std::size_t end = domain.find("]:");

        if( start == std::string::npos && end == std::string::npos)
        {
            return UdpSocket::Connect(Slice<const char>(domain.c_str(), domain.length()), 80, timeout);
        }

        if ( start ==  std::string::npos || end == std::string::npos)
        {
            return nullptr;
        }

        if (  start > end )
        {
            return nullptr;
        }
        std::string host = domain.substr(start + 1, end);   // get from "live" to the end

        std::string ports = domain.substr(end +2); // get from "live" to the end
        if( ports.empty())
            return nullptr;

        int port = atoi(ports.c_str());

        return UdpSocket::Connect(Slice<const char>(host.c_str(), host.length()), port, timeout);
    }
    else if (num == 1)
    {
        std::size_t pos = domain.find(':'); // position of "live" in str
        std::string host = domain.substr(0, pos);   // get from "live" to the end
        std::string ports = domain.substr(pos + 1); // get from "live" to the end
        int port = atoi(ports.c_str());

        return UdpSocket::Connect(Slice<const char>(host.c_str(), host.length()), port, timeout);
    }
    else
    {
        return UdpSocket::Connect(Slice<const char>(domain.c_str(), domain.length()), 80, timeout);
    }
}

UdpSocket * UdpSocket::Connect(SocketAddr host, uint64_t microseconds)
{
    struct timeval timeout{0,0};
    timeout.tv_sec = microseconds / 1000;
    timeout.tv_usec = (microseconds % 1000) * 1000;

    return Connect( std::move(host), timeout);
}

UdpSocket * UdpSocket::Connect(Slice<const char> host, size_t port, uint32_t microseconds)
{
    struct timeval timeout{0, 0};
    timeout.tv_sec = microseconds / 1000;
    timeout.tv_usec = (microseconds % 1000) * 1000;

    return Connect( std::move(host), port, timeout);
}

UdpSocket * UdpSocket::Connect(const std::string& domain, uint32_t microseconds )
{
    std::size_t pos = domain.find(':'); // position of "live" in str
    if (pos > 0)
    {
        std::string host = domain.substr(0, pos);   // get from "live" to the end
        std::string ports = domain.substr(pos + 1); // get from "live" to the end
        // std::cout << host << ' ' << ports << '\n';
        int port = atoi(ports.c_str());
        return UdpSocket::Connect({host.c_str(), host.length()}, port, microseconds);
    }
    else
    {
        return UdpSocket::Connect( {domain.c_str(), domain.length()}, 80, microseconds);
    }
}

UdpSocket::UdpSocket(SOCKET fd)
{
    this->fd = fd;
}
size_t UdpSocket::read(const Slice<uint8_t>& slice)
{
    return recv(this->fd, (char*)slice.addr, slice.len, 0);
}

size_t UdpSocket::write(const Slice<uint8_t>& slice)
{
    return send(this->fd, (char*)slice.addr, slice.len, 0);
}
