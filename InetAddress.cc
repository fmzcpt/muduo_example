#include "InetAddress.h"
#include<iostream>
#include<netdb.h>
#include<assert.h>
#include<arpa/inet.h>
#include<strings.h>
#include<string.h>
#include<vector>
// InetAddress::InetAddress(StringPiece ip,uint16_t port)
// {
    
// }

InetAddress::InetAddress(uint16_t port, bool loopbackonly)
{
    ::bzero(&saddr_,sizeof(saddr_));
    saddr_.sin_family=AF_INET;
    saddr_.sin_port=htons(port);
    saddr_.sin_addr.s_addr=htonl(loopbackonly?INADDR_LOOPBACK:INADDR_ANY);
}

std::string InetAddress::toIp()
{
    // std::string ip="";
    char buf[32]="";
    ::inet_ntop(AF_INET,&saddr_.sin_addr.s_addr,buf,sizeof buf);
    return buf;
}

std::string InetAddress::toIpPort()
{
    char buf[32]="";//"1.1.1.1:2342"
    ::inet_ntop(AF_INET,&saddr_.sin_addr.s_addr,buf,sizeof buf);
    int end=::strlen(buf);
    uint16_t port=portHostEndian();
    snprintf(buf+end,sizeof buf - end,"%u",port);
    return buf;
}

static const int KResolveBufferSize=4096;
bool InetAddress::resolveSlow(const char* hostname,InetAddress* out)
{
    std::vector<char> buf(KResolveBufferSize*2);
    hostent hent;
    hostent* he=NULL;
    ::bzero(&hent,sizeof(hent));
    int herror=0;

    while (buf.size()<=64*KResolveBufferSize)
    {
        /* code */
        int ret=gethostbyname_r(hostname,&hent,buf.data(),buf.size(),&he,&herror);
        // std::cout<<ret<<std::endl;
        if(ret==0 && he!=NULL)
        {
            assert(he->h_addrtype==AF_INET && he->h_length==sizeof(uint32_t));
            out->saddr_.sin_addr=*reinterpret_cast<in_addr*>(he->h_addr_list[0]);
            return true;
        }else if(ret==ERANGE)
        {
            buf.resize(buf.size()*2);
        }else
        {
            if(ret)
            {
                perror("InetAddress resolve");
                return false;
            }
            
        }
        
    }
    return false;

}

bool InetAddress::resolve(StringPiece hostname,InetAddress* out)
{
    std::vector<char> buf(KResolveBufferSize);
    hostent hent;
    hostent* he=NULL;
    ::bzero(&hent,sizeof(hent));
    int herror=0;

    // while (buf.size()<64*KResolveBufferSize)
    // {
    //     /* code */
    int ret=gethostbyname_r(hostname.c_str(),&hent,buf.data(),buf.size(),&he,&herror);
    // std::cout<<ret<<std::endl;
    if(ret==0 && he!=NULL)
    {
        assert(he->h_addrtype==AF_INET && he->h_length==sizeof(uint32_t));
        out->saddr_.sin_addr=*reinterpret_cast<in_addr*>(he->h_addr_list[0]);
        // std::cout<<1<<std::endl;
        return true;
    }else if(ret==ERANGE)
    {
        // buf.resize(buf.size()*2);
        return resolveSlow(hostname.c_str(),out);
    }else
    {
        if(ret)
        {
            perror("InetAddress resolve");
            return false;
        }
        
    }      
    // }
    return false;
}
