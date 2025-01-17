#include <iostream>
#include "net.h"
int test_client(int agrc, char *agrv[])
{
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 10;
    auto stream_pack = TcpStream::Connect("10.112.219.204:80", timeout);
    //auto stream_pack = TcpStream::Connect("[fe80::1b01:c877:1297:9129]:80", timeout);
    if (stream_pack.is_err())
    {
        printf("connect fail�� %s \n", stream_pack.unwrap_err().c_str());
        return -1;
    }

    printf("connect OK \n");
    TcpStream stream = stream_pack.unwrap();


    auto local_addr = stream.local();
    if( local_addr.is_ok())
    {
        std::cout << "--" << local_addr.unwrap().to_string() << std::endl;
    }

    auto peer_addr = stream.peer();
    if( peer_addr.is_ok())
    {
        std::cout << "--" << peer_addr.unwrap().to_string() << std::endl;
    }


    printf("stream socket %u\n", stream.get_socket());

    const char* data = "GET http://10.112.219.204/SDK/language HTTP/1.1\r\n"
                       "Accept: */*\r\n"
                       "If-Modified-Since: 0\r\n"
                       "X-Requested-With: XMLHttpRequest\n"
                       "Referer: http://10.112.219.204/doc/page/login.asp?_1717680778970\r\n"
                       "Accept-Language: zh-Hans-CN,zh-Hans;q=0.5\r\n"
                       "Accept-Encoding: gzip, deflate\r\n"
                       "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko\r\n"
                       "Host: 10.112.219.204\r\n"
                       "Connection: Keep-Alive\r\n"
                       "Cookie: _wnd_size_mode=5\r\n"
                       "\r\n";
    Slice<uint8_t> slice((uint8_t *)data, strlen(data));

    std::cout << "send :" << slice.to_string() << std::endl;

    stream.write(slice);


    char resp[300] = {0};
    Slice<char> slice_resp( &resp[0], 300);

    std::size_t len = stream.read(slice_resp);

    printf("read size %lu\n", len);
    printf("read  %s\n", slice_resp.addr);

    return 0;
}

int test_server(int agrc, char *agrv[])
{
    const char* host = "0.0.0.0";

    auto listener_pack = TcpListener::Bind(Slice<const char>(host, strlen(host)), 9005, 0);
    if (listener_pack.is_err())
    {
        printf("connect fail %s \n", listener_pack.unwrap_err().c_str());
        return 1;
    }
    TcpListener listener = listener_pack.unwrap();

    printf(" listener %d\n", listener.get_socket());

//    auto stream_pack = listener.accept();
//    if (stream_pack.is_err())
//    {
//        printf("Accept fail %d \n", stream_pack.unwrap_err().c_str());
//        return 1;
//    }


    while(1) {
        auto stream_pack = listener.accept(10 * 1000);
        if (stream_pack.is_err())
        {
            printf("Accept fail %s \n", stream_pack.unwrap_err().c_str());
        }
        else {
            printf("Accept OK  \n" );
            break;

        }

    }

//    TcpStream stream = stream_pack.unwrap();
//
//    char data[300] = "0";
//    Slice<uint8_t> slice((uint8_t *)&data[0], 300);
//
//    std::size_t len = stream.read(slice);
//
//    printf("read size %lu\n", len);
//    printf("read  %s\n", slice.addr);
//
//    stream.write(slice);

    return 1;
}
#if 0
int main2(int agrc, char *agrv[])
{
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,2),&wsaData)){
        cout<<"WinSock���ܱ���ʼ��";
        WSACleanup();
        return 0;
    }
    SOCKET sockCli;
    sockCli=socket(AF_INET,SOCK_STREAM,0);
    SOCKADDR_IN addrSer;
    addrSer.sin_family=AF_INET;
    addrSer.sin_port=htons(80);
    addrSer.sin_addr.S_un.S_addr=inet_addr("10.112.219.204");
    int res=connect(sockCli,(SOCKADDR *)&addrSer,sizeof(SOCKADDR));
    if(res){
        cout<<" error -----------------"<<endl;
        return -1;
    }else{
        cout<<" ok -----------------"<<endl;
    }
    char sendbuf[256],recvbuf[256];
    while(1){
        cout<<"client:>";
        cin>>sendbuf;
        if(strcmp(sendbuf,"bye")==0)	break;
        send(sockCli,sendbuf,strlen(sendbuf)+1,0);
        if(recv(sockCli,recvbuf,256,0)>0){
            cout<<"server:>"<<recvbuf<<endl;
        }else{
            cout<<"server close"<<endl;
            break;
        }
    }
    closesocket(sockCli);
    WSACleanup();
    return 0;
}

#endif




int main(int agrc, char *agrv[])
{
//    auto ips = IpAddr::Create(std::string("192.168.1.1"));
//    if( ips.is_err())
//    {
//        printf("Create fail %s \n", ips.unwrap_err().c_str());
//    }
//
//    auto ips1 = IpAddr::Create(std::string("192.256.1.1"));
//    if( ips1.is_err())
//    {
//        printf("1 Create fail %s \n", ips1.unwrap_err().c_str());
//    }
//
//    auto ips2 = IpAddr::Create(std::string("192.1.1"));
//    if( ips2.is_err())
//    {
//        printf("ips2 Create fail %s \n", ips2.unwrap_err().c_str());
//    }
//
//    auto ips3 = IpAddr::Create(std::string("192.01.1.1"));
//    if( ips3.is_err())
//    {
//        printf("ips3 Create fail %s \n", ips3.unwrap_err().c_str());
//    }
//
//
//    auto ips4 = IpAddr::Create(std::string("192.01.1.1aaa"));
//    if( ips4.is_err())
//    {
//        printf("ips4 Create fail %s \n", ips4.unwrap_err().c_str());
//    }
//
//    auto ips5 = IpAddr::Create(std::string("www.baidu.com.cn"));
//    if( ips5.is_err())
//    {
//        printf("ips5 Create fail %s \n", ips5.unwrap_err().c_str());
//    }
//
//    auto ip6 = IpAddr::Create(std::string("2001:0db8:3c4g:0015:0000:0000:1a2f:1a2b"));
//    if( ip6.is_err())
//    {
//        printf("ip6 Create fail %s \n", ip6.unwrap_err().c_str());
//    }
//
//    auto ip7 = IpAddr::Create(std::string("2031:0000:1F11F:0000:0000:0100:11A0:ADDF"));
//    if( ip7.is_err())
//    {
//        printf("ip7 Create fail %s \n", ip7.unwrap_err().c_str());
//    }
//
//    auto ip8 = IpAddr::Create(std::string("1080:0:0:0:8:800:200C:417A1"));
//    if( ip8.is_err())
//    {
//        printf("ip8 Create fail %s \n", ip8.unwrap_err().c_str());
//    }
//
//    auto ip9 = IpAddr::Create(std::string("FF01:0:::101"));
//    if( ip9.is_err())
//    {
//        printf("ip9 Create fail %s \n", ip9.unwrap_err().c_str());
//    }
//
//    auto ip10 = IpAddr::Create(std::string("0:0:0:0:0:0:0::1"));
//    if( ip10.is_err())
//    {
//        printf("ip10 Create fail %s \n", ip10.unwrap_err().c_str());
//    }
//
//    auto ip11 = IpAddr::Create(std::string("0:0:0:0:0:0:0:01234"));
//    if( ip11.is_err())
//    {
//        printf("ip11 Create fail %s \n", ip11.unwrap_err().c_str());
//    }

//    auto addr1 = SocketAddr::Create(std::string("10.112.219.204"));
//    if( addr1.is_ok())
//    {
//        printf("addr1 Create OK %s \n", addr1.unwrap().to_string().c_str());
//    }else
//    {
//       printf("addr1 Create fail %s \n", addr1.unwrap_err().c_str());
//    }
//
//    auto addr2 = SocketAddr::Create(std::string("10.112.219.204:80"));
//    if( addr2.is_ok())
//    {
//        printf("addr2 Create OK %s \n", addr2.unwrap().to_string().c_str());
//    }else
//    {
//        printf("addr2 Create fail %s \n", addr2.unwrap_err().c_str());
//    }


    test_client(agrc, agrv);

    //test_server(agrc, agrv);
}