#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <algorithm>

#include <iostream>
#include "socket_include.h"
#include "TcpStream.h"
#include "TcpListener.h"


Result<TcpListener> TcpListener::Bind(SocketAddr host , int backlog)
{
    auto server = Socket::Create(host.sin4.sin_family, SOCK_STREAM, IPPROTO_TCP);
    if( server.is_err())
    {
        printf("Socket::Create error\n");
        return Err(server.unwrap_err());
    }

    auto _server  = server.unwrap();
    _server.set_reuse(true);

    if(_server.bind(host).is_err())
        return Err(std::string(StrError(Errno)));

    if(listen(_server.get_socket(), backlog) != 0)
    {
        return Err(std::string(StrError(Errno)));
    }

    return Ok(TcpListener(_server.take()));
}


Result<TcpListener> TcpListener::Bind(Slice<const char> host, uint16_t port, int backlog)
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
        for (struct addrinfo *p = res; p != nullptr; p = p->ai_next)
        {
            auto ip_addr =  SocketAddr((struct sockaddr_storage*)p->ai_addr);

            ip_addr.set_port(port);

            std::cout << "ip_addr :" << ip_addr.to_string() << "." << std::endl;

            auto server = Bind(ip_addr);
            if( server.is_err())
                continue;

            stream = server.unwrap().take();
            std::cout << "find "<<std::endl;
            break;
        }

        freeaddrinfo(res);
    }
    else
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return Err(std::string(StrError(Errno)));
    }

    if (stream != INVALID_SOCKET)
    {
        if (::listen(stream, 24) != 0)
        {
            fprintf(stdout, "%s:%d socket_listen failed: errno %d\n", __FILE__, __LINE__, errno);
            close(stream);
            return Err(std::string(StrError(Errno)));
        }

        return Ok(TcpListener(stream));
    }

    return Err(std::string(StrError(Errno)));
}


Result<TcpListener> TcpListener::Bind(std::string domain, uint16_t port, int backlog) {
    return Bind(Slice<const char>{domain.c_str(), domain.length()}, port, 0);
}

Result<TcpListener> TcpListener::Bind(std::string domain, int backlog) {

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


TcpListener::TcpListener(SOCKET fd) : Socket(fd)
{
}

TcpListener::TcpListener(TcpListener &&other) : Socket(other.fd)
{
    other.fd  = -1;
}


Result<TcpStream> TcpListener::accept( struct timeval timeout)
{
    if( timeout.tv_sec == 0 && timeout.tv_usec == 0)
    {
        struct sockaddr_storage clent_addr;
        int addr_size = sizeof(struct sockaddr_storage);

        int connect_fd = ::accept(this->fd, (struct sockaddr *)&clent_addr, (socklen_t *)&addr_size);
        if (connect_fd < 0)
        {
            fprintf(stdout, "%s:%d accept failed: [errno  \n", __FILE__, __LINE__);
            return Err(std::string(StrError(Errno)));
        }

        //return Ok(std::pair<TcpStream, SocketAddr>{TcpStream(connect_fd), SocketAddr(clent_addr)});
        return Ok(TcpStream(connect_fd));
    }
    /* 如果有超时时间，调用select判断在超时时间内，是否有数据传输进来 */
//    struct timeval timeout;
//    timeout.tv_sec = msecond / 1000;
//    timeout.tv_usec = (msecond % 1000) * 1000;

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);

    do {
        int ret = ::select(fd + 1, &fdset, nullptr, nullptr, &timeout);

        /* 在指定时间内，若检测到端口可读，先不做处理，等待后续使用accept接收数据，否则直接返回 */
        if (ret > 0) {
            if (FD_ISSET(fd, &fdset)) {
                break;
            } else {
                return Err(std::string(StrError(Errno)));
            }
        } else if (0 == ret)
        {
            return Err(std::string("time out"));
        } else {
#ifdef  __WINDOWS__
            if( Errno == WSAEWOULDBLOCK)
#else
                if( Errno == EINTR)
#endif
            {
                continue;
            }

            return Err(std::string(StrError(Errno)));
        }
    }while(0);


    struct sockaddr_storage clent_addr;
    int addr_size = sizeof(struct sockaddr_storage);

    int connect_fd = ::accept(this->fd, (struct sockaddr *)&clent_addr, (socklen_t *)&addr_size);
    if (connect_fd < 0)
    {
        fprintf(stdout, "%s:%d accept failed: [errno  \n", __FILE__, __LINE__);
        return Err(std::string(StrError(Errno)));
    }

    //return Ok(std::pair<TcpStream, SocketAddr>{TcpStream(connect_fd), SocketAddr(clent_addr)});
    return Ok(TcpStream(connect_fd));
}

Result<TcpStream> TcpListener::accept(uint32_t msecond)
{
    if( msecond == 0)
    {
        return accept({0,0});
    }
    /* 如果有超时时间，调用select判断在超时时间内，是否有数据传输进来 */
    struct timeval timeout;
    timeout.tv_sec = msecond / 1000;
    timeout.tv_usec = (msecond % 1000) * 1000;

    return accept(timeout);
}



