#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <algorithm>
#include <iostream>
#include "socket_include.h"
#include "TcpStream.h"


Result<TcpStream> TcpStream::Connect(SocketAddr &addr, struct timeval timeout)
{
    int family = addr.is_v4() ? AF_INET : AF_INET6;
    auto client = Socket::Create(family, SOCK_STREAM, IPPROTO_TCP);
    if( client.is_err())
    {
        return Err(client.unwrap_err());
    }

    auto _client  = client.unwrap();

    if( timeout.tv_sec == 0 && timeout.tv_usec == 0)
    {
        auto ret = _client.connect(addr);

        if( ret.is_err())
        {
            return Err(ret.unwrap_err());
        }

        return Ok(TcpStream(_client.take()));
    }

    // 清除错误
    int opt_val = 0;
    int length = sizeof(opt_val);
    auto socket_fd = _client.get_socket( );

    _client.set_nodelay(true);

    auto ret = _client.connect(addr);
    if( ret.is_err())
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

    _client.set_nodelay(0);

    if (0 == opt_val)
    {
        return Ok(TcpStream(_client.take()));
    }
    else{
        return Err(client.unwrap_err());
    }
}

Result<TcpStream> TcpStream::Connect(Slice<const char> host, size_t port, struct timeval timeout)
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
            if( client.is_err())
                continue;

            stream = client.unwrap().take();
            break;
        }
        freeaddrinfo(res);
    }
    else
    {
        auto _ips =  SocketAddr::Create( host, port);
        if( _ips.is_err()){
            return Err(_ips.unwrap_err());
        }

        auto ips= _ips.unwrap();
        ips.set_port(port);

        auto client = Connect(ips, timeout);
        if( client.is_err())
            return Err(_ips.unwrap_err());

        stream = client.unwrap().take();
    }

    if( stream == INVALID_SOCKET)
        return Err(std::string(StrError(Errno)));

    return Ok(TcpStream(stream));
}

Result<TcpStream> TcpStream::Connect(const std::string& domain, struct timeval timeout)
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
            return Err(std::string ("Illegal string"));
        }

        if (  start > end )
        {
            return Err(std::string ("Illegal string"));
        }
        std::string host = domain.substr(start + 1, end);   // get from "live" to the end

        std::string ports = domain.substr(end +2); // get from "live" to the end
        if( ports.empty())
            return Err(std::string ("Illegal string"));

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


Result<TcpStream> TcpStream::Connect(SocketAddr &addr, uint32_t microseconds)
{
    struct timeval timeout{0,0};
    timeout.tv_sec = microseconds / 1000;
    timeout.tv_usec = (microseconds % 1000) * 1000;

    return Connect( addr, timeout);
}

Result<TcpStream> TcpStream::Connect(Slice<const char> host, size_t port, uint32_t microseconds )
{
    struct timeval timeout{0, 0};
    timeout.tv_sec = microseconds / 1000;
    timeout.tv_usec = (microseconds % 1000) * 1000;

    return Connect( std::move(host), port, timeout);
}

Result<TcpStream> TcpStream::Connect(const std::string& domain, uint32_t microseconds )
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
