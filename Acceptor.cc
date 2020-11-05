#include "Acceptor.h"

#include<sys/socket.h>
#include<stdio.h>
Acceptor::Acceptor(const InetAddress& addr):sock_(Socket::createTcp())
{
    sock_.setReuseAddr(true);
    sock_.bindOrdie(addr);
    sock_.listenOrdie();
}

TcpStreamPtr Acceptor::accept()
{
    TcpStreamPtr stream;
    int ac=::accept(sock_.fd(),NULL,NULL);
    if(ac>0)
    {
        stream.reset(new TcpStream(Socket(ac)));
        return stream;
    }else
    {
        perror("accept");
        return TcpStreamPtr();
    }
}

