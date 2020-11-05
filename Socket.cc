#include "Socket.h"
#include "InetAddress.h"
#include<assert.h>
#include<string.h>
#include<strings.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<unistd.h>

namespace
{
    typedef sockaddr SA;

    const SA* sockaddr_cast(const sockaddr_in* addr)
    {
        return static_cast<const SA*>(implict_cast<const void*>(addr));
    }

    SA* sockaddr_cast(sockaddr_in* addr)
    {
        return static_cast<SA*>(implict_cast<void*>(addr));
    }
}

Socket::~Socket()
{
    if(sockfd_>=0)
    {
        int ret=::close(sockfd_);
        assert(ret==0);(void)ret;
    }
}

void Socket::bindOrdie(const InetAddress& addr)
{
    sockaddr_in saddr=addr.getSockAddrInet();
    int ret=::bind(sockfd_,sockaddr_cast(&saddr),sizeof saddr);
    if(ret)
    {
        perror("Socket bindOrdie");
    }
    return;
}

void Socket::listenOrdie()
{
    // sockaddr_in saddr=addr.getSockAddrInet();
    int ret=::listen(sockfd_,SOMAXCONN);
    if(ret)
    {
        perror("Socket Listen");
        abort();
    }
    return;

}

int Socket::connect(const InetAddress& addr)
{
    sockaddr_in saddr=addr.getSockAddrInet();
    int ret=::connect(sockfd_,sockaddr_cast(&saddr),sizeof saddr);
    if(ret==-1)
    {
        perror("Socket connect");
        abort();
    }
    return ret;
}

void Socket::shutdownWrite()
{
    int ret=::shutdown(sockfd_,SHUT_WR);
    if(ret==-1)
    {
        perror("Socket shutdownWrite");
    }
}

void Socket::setReuseAddr(bool on)
{
    int opt=on?1:0;
    int ret=::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof opt);
    if(ret==-1)
    {
        perror("Socket setreuse");
    }
    return;
}

void Socket::setNodelay(bool on)
{
    int opt=on?1:0;
    int ret=::setsockopt(sockfd_,IPPROTO_TCP,TCP_NODELAY,&opt,sizeof opt);
    if(ret==-1)
    {
        perror("Socket setNodelay");
    }
    return;
}

InetAddress Socket::getLocalAddr()const
{
    sockaddr_in local;
    ::bzero(&local,sizeof local);
    socklen_t len=static_cast<socklen_t>(sizeof local);
    if(::getsockname(sockfd_,sockaddr_cast(&local),&len))
    {
        perror("Socket getlocalname");
        abort();
    }
    return InetAddress(local);
}

InetAddress Socket::getPeerAddr() const
{
    sockaddr_in local;
    ::bzero(&local,sizeof local);
    socklen_t len=static_cast<socklen_t>(sizeof local);
    if(::getpeername(sockfd_,sockaddr_cast(&local),&len))
    {
        perror("Socket getpeername");
        abort();
    }
    return InetAddress(local);
}

int Socket::read(void* buf,int len)
{
    int ret=::read(sockfd_,buf,len);
    return ret;
}   

int  Socket::write(const void* buf,int len)
{
    int ret=::write(sockfd_,buf,len);
    return ret;
}

Socket Socket::createTcp()
{
    int fd=::socket(AF_INET,SOCK_STREAM|SOCK_CLOEXEC,IPPROTO_TCP);
    assert(fd>=0);
    return Socket(fd);
}

Socket Socket::createUdp()
{
    int fd=::socket(AF_INET,SOCK_DGRAM|SOCK_CLOEXEC,IPPROTO_UDP);
    assert(fd>=0);
    return Socket(fd);
}

