#ifndef MUDUO_EXAMPLE_INETADDRESS_H
#define MUDUO_EXAMPLE_INETADDRESS_H
#include "Common.h"
#include <string>
#include <vector>
#include <netinet/in.h>
class InetAddress{
    
    public:
        InetAddress(const sockaddr_in& saddr):saddr_(saddr)
        {

        }
        InetAddress(uint16_t port,bool loopbackonly=false);
        InetAddress(StringPiece ip,uint16_t port);
        InetAddress(StringPiece ipPort);

        const sockaddr_in& getSockAddrInet() const 
        {
            return saddr_;
        }
        void setSockAddrInet(const sockaddr_in& saddr)
        {
            saddr_=saddr;
        }
        void setPort(uint16_t port)
        {
            saddr_.sin_port=port;
        }

        std::string toIp();
        std::string toIpPort();

        uint32_t ipNetEndian() const
        {
            return saddr_.sin_addr.s_addr;
        }
        uint16_t portNetEndian() const
        {
            return saddr_.sin_port;
        }

        uint32_t ipHostEndian() const
        {
            return ntohl(saddr_.sin_addr.s_addr);
        }
        uint16_t portHostEndian()const
        {
            return ntohs(saddr_.sin_port);
        }

        static bool resolve(StringPiece hostname,InetAddress*);
        static std::vector<InetAddress> resolveAll(StringPiece hostname,uint16_t port=0);

        bool operator==(const InetAddress& rhs) const
        {
            return saddr_.sin_family==rhs.saddr_.sin_family &&
                    ipNetEndian()==rhs.ipNetEndian()&&
                    portNetEndian()==rhs.portNetEndian();
        }


    

    private:
        static bool resolveSlow(const char* hostname,InetAddress*);
        sockaddr_in saddr_; 

};

#endif