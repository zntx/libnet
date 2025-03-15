#ifndef NET_TCPSTREAM_H
#define NET_TCPSTREAM_H

/*


1.0.0 · source · [−]
*/
#include <chrono>
#include <string>
#include <stdint.h>
#include "Slice.h"
#include "Result.h"
#include "SocketAddr.h"
#include "Socket.h"
#include "Array.h"

using namespace std;
using namespace std::chrono;

class TcpStream : public Socket
{

public:
    static Result<TcpStream> Connect(SocketAddr &addr, struct timeval  = {0,0});
    static Result<TcpStream> Connect(Slice<const char> host, size_t port, struct timeval  = {0,0});
    static Result<TcpStream> Connect(const std::string& domain, struct timeval = {0,0});

    static Result<TcpStream> Connect(SocketAddr &addr, uint32_t microseconds = 0);
    static Result<TcpStream> Connect(Slice<const char> host, size_t port, uint32_t microseconds = 0);
    static Result<TcpStream> Connect(const std::string& domain, uint32_t microseconds = 0);

    template<typename Rep, typename Period>
    static Result<TcpStream> Connect( SocketAddr &host, const chrono::duration<Rep, Period>& _rtime) {
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
    static Result<TcpStream> Connect( std::string &domain, const chrono::duration<Rep, Period>& _rtime) {
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
    static Result<TcpStream> Connect( Slice<const char> &host, size_t port, const chrono::duration<Rep, Period>& _rtime) {
        if (_rtime <= _rtime.zero())
            return Connect(host, port ,0);
        auto _s = chrono::duration_cast<chrono::seconds>(_rtime);
        auto _us = chrono::duration_cast<chrono::microseconds>(_rtime - _s);

        struct timeval timeout{0,0};
        timeout.tv_sec = _s;
        timeout.tv_usec = _us;

        return Connect(host, port , timeout);
    }


    ~TcpStream() override;
    explicit TcpStream(SOCKET fd);
    TcpStream(TcpStream &&other)  ;
    TcpStream(TcpStream &other) = delete;
    TcpStream(const TcpStream &other) = delete;
    TcpStream &operator=(TcpStream &&other)  ;

    size_t write(Slice<uint8_t> &slice);
    Result<size_t> read(Slice<char> &slice);
    size_t read(Slice<uint8_t> &slice);

    Result<bool> read_line(Array<char> &buf);

    /* Returns the socket address of the remote peer of this TCP connection. */
    //Result<SocketAddr, int> peer_addr();
    //Result<SocketAddr, int> local_addr();

    // Result<void, string> shutdown(Shutdown how);
    Result<TcpStream, int> try_clone();
    // Result<void, string> set_read_timeout(struct timeval *tv);
    // Result<void, string> set_read_timeout(Option<duration> dur);
    // Result<void, string> set_write_timeout(struct timeval *tv);
    // Result<void, string> set_write_timeout(Option<duration> dur);
    //Result<Option<struct timeval>> read_timeout();
    //Result<Option<struct timeval>> write_timeout();
    // Result<Option<duration>, Error> read_timeout();
    // Result<Option<duration>, Error> write_timeout();
    // Result<void, string> set_linger(linger : Option<duration>);
    // Result<Option<duration>, Err> linger();
    // Result<void, string> set_nodelay(bool nodelay  );
    //Result<bool> nodelay();
    //Result<void, int> set_ttl(uint32_t ttl);
    //Result<uint32_t> ttl();
    //Result<void, int> take_error();
    //Result<void, int> set_nonblocking(bool nonblocking);
};


#endif