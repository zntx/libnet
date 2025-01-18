#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <algorithm>
#include <iostream>
#include "../include/socket_include.h"
#include "../include/TcpStream.h"


TcpStream* TcpStream::Connect(SocketAddr* addr, struct timeval timeout)
{
    int family = addr->is_v4() ? AF_INET : AF_INET6;
    auto client = Socket::Create(family, SOCK_STREAM, IPPROTO_TCP);
    if( client == nullptr)
    {
        return nullptr;
    }

    if( timeout.tv_sec == 0 && timeout.tv_usec == 0)
    {
        auto ret = client->connect(addr);

        if( !ret )
        {
            return nullptr;
        }

        return new(std::nothrow) TcpStream(client->take());
    }

    // 清除错误
    int opt_val = 0;
    int length = sizeof(opt_val);
    auto socket_fd = client->get_socket( );

    client->set_nodelay(true);

    auto ret = client->connect(addr);
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

            if(::select(socket_fd + 1, NULL, &set, NULL, &timeout) > 0)
            {
                getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, (char*)&opt_val, (socklen_t*)&length);
            }
        }
    }

    client->set_nodelay(0);

    if (0 == opt_val)
    {
        return new(std::nothrow) TcpStream(client->take());
    }
    else{
        return nullptr;
    }
}



TcpStream * TcpStream::Connect(SocketAddr &addr, struct timeval timeout)
{
    int family = addr.is_v4() ? AF_INET : AF_INET6;
    auto client = Socket::Create(family, SOCK_STREAM, IPPROTO_TCP);
    if( client == nullptr)
    {
        return nullptr;
    }

    if( timeout.tv_sec == 0 && timeout.tv_usec == 0)
    {
        auto ret = client->connect(addr);

        if( !ret )
        {
            return nullptr;
        }

        return new(std::nothrow) TcpStream(client->take());
    }

    // 清除错误
    int opt_val = 0;
    int length = sizeof(opt_val);
    auto socket_fd = client->get_socket( );

    client->set_nodelay(true);

    auto ret = client->connect(addr);
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

            if(::select(socket_fd + 1, NULL, &set, NULL, &timeout) > 0)
            {
                getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, (char*)&opt_val, (socklen_t*)&length);
            }
        }
    }

    client->set_nodelay(0);

    if (0 == opt_val)
    {
        return new(std::nothrow) TcpStream(client->take());
    }
    else{
        return nullptr;
    }
}

TcpStream * TcpStream::Connect(Slice<const char> host, size_t port, struct timeval timeout)
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

    int n = getaddrinfo(host.to_string().c_str(), nullptr, &hints, &res);
    if (!n)
    {
        for (struct addrinfo *ai = res ; ai != nullptr; ai = ai -> ai_next)
        {
            auto ip_addr =  SocketAddr((struct sockaddr_storage*)ai->ai_addr);
            ip_addr.set_port(port);

            auto client = Connect(ip_addr, timeout);
            if( client == nullptr)
                continue;

            stream = client->take();
            break;
        }
        freeaddrinfo(res);
    }
    else
    {
        auto ips =  SocketAddr::Create( host, port);
        if( ips == nullptr ){
            return nullptr;
        }

        ips->set_port(port);

        auto client = Connect(ips, timeout);
        if( client == nullptr)
            return nullptr;

        stream = client->take();
    }

    if( stream == INVALID_SOCKET)
        return nullptr;

    return new(std::nothrow) TcpStream(stream);
}

TcpStream * TcpStream::Connect(const std::string& domain, struct timeval timeout)
{
    int num = std::count(domain.begin(),domain.end(),':');
    std::cout << " std::count " << num << std::endl;
    if( num >= 2)
    {
        std::size_t start = domain.find('[');
        std::size_t end = domain.find("]:");

        if( start == std::string::npos && end == std::string::npos)
        {
            return TcpStream::Connect(Slice<const char>(domain.c_str(), domain.length()), 80, timeout);
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

        return TcpStream::Connect(Slice<const char>(host.c_str(), host.length()), port, timeout);
    }
    else if (num == 1)
    {
        std::size_t pos = domain.find(':'); // position of "live" in str
        std::string host = domain.substr(0, pos);   // get from "live" to the end
        std::string ports = domain.substr(pos + 1); // get from "live" to the end
        int port = atoi(ports.c_str());

        return TcpStream::Connect(Slice<const char>(host.c_str(), host.length()), port, timeout);
    }
    else
    {
        return TcpStream::Connect(Slice<const char>(domain.c_str(), domain.length()), 80, timeout);
    }
}


TcpStream * TcpStream::Connect(SocketAddr &addr, uint32_t microseconds)
{
    struct timeval timeout{0,0};
    timeout.tv_sec = microseconds / 1000;
    timeout.tv_usec = (microseconds % 1000) * 1000;

    return Connect( addr, timeout);
}

TcpStream * TcpStream::Connect(Slice<const char> host, size_t port, uint32_t microseconds )
{
    struct timeval timeout{0, 0};
    timeout.tv_sec = microseconds / 1000;
    timeout.tv_usec = (microseconds % 1000) * 1000;

    return Connect( std::move(host), port, timeout);
}

TcpStream * TcpStream::Connect(const std::string& domain, uint32_t microseconds )
{
    std::size_t pos = domain.find(':'); // position of "live" in str
    if (pos > 0)
    {
        std::string host = domain.substr(0, pos);   // get from "live" to the end
        std::string ports = domain.substr(pos + 1); // get from "live" to the end
        // std::cout << host << ' ' << ports << '\n';
        int port = atoi(ports.c_str());
        return TcpStream::Connect({host.c_str(), host.length()}, port, microseconds);
    }
    else
    {
        return TcpStream::Connect( {domain.c_str(), domain.length()}, 80, microseconds);
    }
}



TcpStream::~TcpStream()
{
    if (this->fd != INVALID_SOCKET) {
        std::cout << "~TcpStream():  close() " << this->fd << std::endl;
        close(this->fd);

        this->fd= INVALID_SOCKET;
    }
}

TcpStream::TcpStream(SOCKET fd)
{
    this->fd = fd;
}

TcpStream::TcpStream(TcpStream &&other)
 {
    this->fd = other.fd;
    other.fd = INVALID_SOCKET;
}

TcpStream &TcpStream::operator=(TcpStream &&other)
{
    this->fd = other.fd;
    other.fd = -1;

    return *this;
}

size_t TcpStream::read(Slice<uint8_t> &slice)
{
    return recv(this->fd, (char*)slice.addr, slice.len, 0);
}

int TcpStream::read(Slice<char> &slice)
{
    return recv(this->fd, slice.addr, slice.len, 0);
}

size_t TcpStream::write(Slice<uint8_t> &slice)
{
    return send(this->fd,  (char*)slice.addr, slice.len, 0);
}
