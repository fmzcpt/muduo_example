#include "Acceptor.h"
#include "InetAddress.h"
#include "Socket.h"

#include <thread>
#include <unistd.h>

#include <iostream>
#include <string>
#include<string.h>

#include<time.h>
#include<sys/time.h>
using namespace std;
struct option
{
    string hostname;
    uint16_t port;
    bool receive;
    bool transmit;
    option():port(3009),receive(0),transmit(0){}    
};

struct Message
{
    int64_t request;
    int64_t reponse;
};
 int64_t now()
 {
    timeval tv={0,0};
    gettimeofday(&tv,NULL);
    return tv.tv_sec*int64_t(1000000)+tv.tv_usec;
 }
void receive(option& opt)
{
    InetAddress addr(opt.port);
    Socket sock(Socket::createUdp());
    sock.bindOrdie(addr);
    while(true)
    {   
        Message msg={0,0};
        sockaddr saddr;
        ::bzero(&saddr,sizeof saddr);
        socklen_t len=sizeof saddr;
        ssize_t re=::recvfrom(sock.fd(),&msg,sizeof msg,0,&saddr,&len);
        if(re==sizeof msg)
        {
            msg.reponse=now();
            ssize_t nw=::sendto(sock.fd(),&msg,sizeof msg,0,&saddr,len);
            if(nw<0)
            {
                perror("receive  send msg");
            }else if (nw!=sizeof msg)
            {
                cout<<"The true send length is "<<nw<<endl;
            }
        }else if(re<0)
        {
            perror("receive msg");
        }else
        {
            cout<<"The true length read is "<<re<<endl;
        }
        
    }
    return;
}

void transmit(option& opt)
{
    InetAddress addr(opt.port);
    Socket sock(Socket::createUdp());
    if(!InetAddress::resolve(opt.hostname,&addr))
    {
        perror("resolve");
        return;
    }
    sock.connect(addr);

    // while(true)
    // {
    thread thr([&sock](){
        while(true)
        {
            Message msg={0,0};
            msg.request=now();
            sock.write(&msg,sizeof msg);
            ::usleep(200*1000);
        }
    });
    // }

    while(true)
    {
        Message msg={0,0};
        int nr=sock.read(&msg,sizeof msg);
        if(nr==sizeof msg)
        {
            int64_t back=now();
            int64_t mine=(back+msg.request)/2;
            cout<<"now:"<<back<<" roundtrip:"<<back-msg.request<<" clock error"<<msg.reponse-mine<<endl;
        }
    }
    return;

}
int main(int argc,char** argv)
{
    int ch=0;
    option opt;
    while((ch=getopt(argc,argv,"rth:"))!=-1)
    {
        switch (ch)
        {
        case /* constant-expression */'r':
            /* code */
            opt.receive=true;
            break;
        case 't':
            opt.transmit=true;
            break;
        case 'h':
            opt.hostname=string(optarg);
            break;
        default:
            break;
        }
    }

    if(opt.receive==opt.transmit)
    {
        cout<<"please only choose one model"<<endl;
        return 0;
    }

    if(opt.receive)
    {
        receive(opt);
    }else
    {
        transmit(opt);
    }
    
}


