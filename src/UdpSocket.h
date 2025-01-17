#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include <chrono>
#include <string>
#include <stdint.h>
#include "Slice.h"
#include "Result.h"
#include "SocketAddr.h"
#include "Socket.h"

using namespace std;
using namespace std::chrono;

class UdpSocket : public Socket
{
public:
    static Result<UdpSocket> Bind(SocketAddr host, int backlog=24);
    static Result<UdpSocket> Bind(Slice<const char> host, uint16_t port, int backlog=24);
    static Result<UdpSocket> Bind(std::string domain, uint16_t port, int backlog=24);
    static Result<UdpSocket> Bind(std::string domain, int backlog=24);

    static Result<UdpSocket> Connect(SocketAddr host, struct timeval timeout = {0,0});
    static Result<UdpSocket> Connect(const std::string& domain, struct timeval timeout = {0,0});
    static Result<UdpSocket> Connect(Slice<const char> host, size_t port, struct timeval timeout = {0,0});

    static Result<UdpSocket> Connect(SocketAddr host, uint64_t microseconds = 0);
    static Result<UdpSocket> Connect(const std::string& domain, uint32_t microseconds = 0);
    static Result<UdpSocket> Connect(Slice<const char> host, size_t port, uint32_t microseconds = 0);

    template<typename Rep, typename Period>
    static Result<UdpSocket> Connect( SocketAddr &host, const chrono::duration<Rep, Period>& _rtime) {
        if (_rtime <= _rtime.zero())
            return Connect(host ,0);
        auto _s = chrono::duration_cast<chrono::seconds>(_rtime);
        auto _us = chrono::duration_cast<chrono::microseconds>(_rtime - _s);

        struct timeval timeout{0,0};
        timeout.tv_sec = _s;
        timeout.tv_usec = _us;

        return Connect(host, timeout);
    }

    template<typename Rep, typename Period>
    static Result<UdpSocket> Connect( std::string &domain, const chrono::duration<Rep, Period>& _rtime) {
        if (_rtime <= _rtime.zero())
            return Connect(domain ,0);
        auto _s = chrono::duration_cast<chrono::seconds>(_rtime);
        auto _us = chrono::duration_cast<chrono::microseconds>(_rtime - _s);

        struct timeval timeout{0,0};
        timeout.tv_sec = _s;
        timeout.tv_usec = _us;

        return Connect(domain, timeout);
    }

    template<typename Rep, typename Period>
    static Result<UdpSocket> Connect( Slice<const char> &host, size_t port, const chrono::duration<Rep, Period>& _rtime) {
        if (_rtime <= _rtime.zero())
            return Connect(host, port ,0);
        auto _s = chrono::duration_cast<chrono::seconds>(_rtime);
        auto _us = chrono::duration_cast<chrono::microseconds>(_rtime - _s);

        struct timeval timeout{0,0};
        timeout.tv_sec = _s;
        timeout.tv_usec = _us;

        return Connect(host, port , timeout);
    }

    explicit UdpSocket(SOCKET fd);
    size_t write(const Slice<uint8_t>& slice);
    size_t read(const Slice<uint8_t>& slice);
    /* Returns the socket address of the remote peer of this TCP connection. */
    //Result<SocketAddr> peer_addr();
    //Result<SocketAddr> local_addr();

    //Result<void, string> shutdown(Shutdown how);
    Result<UdpSocket,int> try_clone();
   // Result<void, string> set_read_timeout(struct timeval *tv);
    //Result<void, string> set_read_timeout(Option<duration> dur);
    //Result<void, string> set_write_timeout(struct timeval *tv);
   // Result<void, string> set_write_timeout(Option<duration> dur);
    //Result<Option<struct timeval>,int> read_timeout();
    //Result<Option<struct timeval>,int> write_timeout();
    //Result<Option<duration>, Error> read_timeout();
    //Result<Option<duration>, Error> write_timeout();
    //Result<void, string> set_linger(linger : Option<duration>);
    //Result<Option<duration>, Err> linger();
   // Result<void, string> set_nodelay(bool nodelay  );
    //Result<bool,int> nodelay();
    //Result<void,int> set_ttl(uint32_t ttl  );
    //Result<uint32_t, int> ttl();
    //Result<void,int> take_error();
    //Result<void,int> set_nonblocking(bool nonblocking);
};

#endif