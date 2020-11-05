#ifndef MUDUO_EXAMPLE_TCPSTREAM_H
#define MUDUO_EXAMPLE_TCPSTREAM_H
#include"Socket.h"
#include<memory>
class InetAddress;
class TcpStream;
typedef std::unique_ptr<TcpStream> TcpStreamPtr;

class TcpStream:noncopyable
{
    public:
        TcpStream(Socket&& sock);
        TcpStream(TcpStream&&)=default;
        TcpStream& operator=(TcpStream&&)=default;
        ~TcpStream()=default;


        static TcpStreamPtr connect(const InetAddress& serverAddr);
        static TcpStreamPtr connect(const InetAddress& serverAddr,const InetAddress& localAddr);

        int receiveAll(void* buf,int len);
        int receiveSome(void* buf,int len);

        int sendAll(const void* buf,int len);
        int sendSome(const void* buf,int len);

        void setTcpNodelay(bool on);
        void shutdown();


    private:
        static TcpStreamPtr connectInter(const InetAddress& serverAddr,const InetAddress* localAddr);
        Socket sock_;
};

#endif