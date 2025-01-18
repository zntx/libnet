//
// Created by zhangyuzhu8 on 2024/6/7.
//

#ifndef NET_TCPLISTENER_H
#define NET_TCPLISTENER_H

#include <chrono>

#include <string>
#include <stdint.h>
#include "Slice.h"
#include "Result.h"
#include "SocketAddr.h"
#include "Socket.h"
#include "TcpStream.h"

using namespace std;
using namespace std::chrono;

class TcpListener : public Socket
{

public:
    static Result<TcpListener> Bind(SocketAddr host , int backlog = 24);
    static Result<TcpListener> Bind(Slice<const char> host, uint16_t port = 80, int backlog=24);
    static Result<TcpListener> Bind(std::string domain, uint16_t port = 80, int backlog=24);
    static Result<TcpListener> Bind(std::string domain, int backlog=24);

    explicit TcpListener(SOCKET fd);
    TcpListener(TcpListener &other) = delete;
    TcpListener(const TcpListener &other) = delete;
    TcpListener(TcpListener &&other)  ;


    Result<std::pair<TcpStream, SocketAddr>> accept(uint32_t msecond = 0);
    Result<std::pair<TcpStream, SocketAddr>> accept(struct timeval timeout ={0,0});

    template<typename Rep, typename Period>
    Result<std::pair<TcpStream, SocketAddr>> accept(  const chrono::duration<Rep, Period>& rtime) {
        if (rtime <= rtime.zero())
            return accept( 0);

        auto _s = chrono::duration_cast<chrono::seconds>(rtime);
        auto _us = chrono::duration_cast<chrono::microseconds>(rtime - _s);

        struct timeval timeout{0,0};
        timeout.tv_sec = _s;
        timeout.tv_usec = _us;

        return accept( timeout);
    }


};


#endif //NET_TCPLISTENER_H
