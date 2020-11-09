#include "TcpStream.h"
#include "InetAddress.h"

#include<unistd.h>
#include<sys/socket.h>
#include<errno.h>
#include<signal.h>

namespace{
    class ignoreSign{
        public:
            ignoreSign()
            {
                ::signal(SIGPIPE,SIG_IGN);
            }
    }initObj;
    bool isSelfConnection(const Socket& sock)
    {
        return sock.getLocalAddr()==sock.getPeerAddr();
    }
}
TcpStream::TcpStream(Socket&& sock):sock_(std::move(sock))
{

}

int TcpStream::receiveAll(void* buf,int len)
{
    return ::recv(sock_.fd(),buf,len,MSG_WAITALL);
}

int TcpStream::receiveSome(void* buf,int len)
{
    return sock_.read(buf,len);
}

int TcpStream::sendSome(const void* buf,int len)
{
    return sock_.write(buf,len);
}

int TcpStream::sendAll(const void* buf,int len)
{
    int written=0;
    while (written<len)
    {
        /* code */
        int ret=sock_.write(static_cast<const char*>(buf)+written,len);
        if(ret>0)
        {
            written+=ret;
        }else if(ret==0)
        {
            break;
        }else if(errno!=EINTR)
        {
            break;
        }
    }
    return written;
    
}

TcpStreamPtr TcpStream::connect(const InetAddress& serveraddr)
{
    return connectInter(serveraddr,nullptr);
}

TcpStreamPtr TcpStream::connect(const InetAddress& serveraddr,const InetAddress& localAddr)
{
    return connectInter(serveraddr,&localAddr);
}

TcpStreamPtr TcpStream::connectInter(const InetAddress& serverAddr,const InetAddress* localAddr)
{
    TcpStreamPtr stream;
    Socket sock(Socket::createTcp());
    if(localAddr)
    {
        sock.bindOrdie(*localAddr);
    }

    if(sock.connect(serverAddr)==0 && !isSelfConnection(sock))
    {
        stream.reset(new TcpStream(std::move(sock)));
    }
    return stream;
}

void TcpStream::shutdown()
{
    sock_.shutdownWrite();
    return;
}