#ifndef MUDUO_EXAMPLE_SOCKET_T
#define MUDUO_EXAMPLE_SOCKET_T
#include "Common.h"
#include  "InetAddress.h"

#include<utility>

class Socket:noncopyable
{
    public:
        Socket(int sockfd):sockfd_(sockfd){}
        Socket(Socket&& rhs):sockfd_(rhs.sockfd_)
        {
            rhs.sockfd_=-1;
        }
        Socket& operator = (Socket&& rhs)
        {
            swap(rhs);
            return *this;
        }

        void swap(Socket& rhs)
        {
            std::swap(sockfd_,rhs.sockfd_);
            return;
        }
        ~Socket();

        int fd(){return sockfd_;}

        //socket api
        void bindOrdie(const InetAddress&);
        void listenOrdie();

        int connect(const InetAddress&);
        void shutdownWrite();

        void setReuseAddr(bool on);
        void setNodelay(bool on);

        InetAddress getPeerAddr() const;
        InetAddress getLocalAddr() const;

        int read(void* buf,int len);
        int write(const void* buf,int len);


        //factory methods
        static Socket createTcp();
        static Socket createUdp();
        
    private:
        int sockfd_;
};
#endif