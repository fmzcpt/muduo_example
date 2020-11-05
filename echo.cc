#include"Acceptor.h"
#include"TcpStream.h"
#include"InetAddress.h"
#include<string>
#include<iostream>
#include<vector>
#include<unistd.h>
#include<thread>
using namespace std;
struct  option;
void receive(option);
void transmit(option);

struct option
{
    /* data */
    uint16_t port;
    int length;
    std::string hostname;
    bool receive,transmit;
};

void receive(option opt)
{
    InetAddress addr(opt.port);
    Acceptor ac(addr);
    cout<<"Accpt..."<<addr.toIpPort()<<endl;
    int count=0;
    while(true)
    {
        TcpStreamPtr tcpstream=ac.accept();
        count++;
        std::thread thr([count](TcpStreamPtr stream){

            char buf[4096];
            int nr=0;
            while (nr=stream->receiveAll(buf,sizeof buf))
            {
                /* code */
                int nw=stream->sendAll(buf,sizeof buf);
                if(nw<nr)
                {
                    break;
                }
            }
            std::cout<<" thread no."<<count;

        },std::move(tcpstream));
        thr.detach();
    }
    
    return;
}

void transmit(option opt)
{
    InetAddress addr(opt.port);
    if(!InetAddress::resolve(opt.hostname,&addr))
    {
        std::cout<<"cannot resolve the hostname";
        return;
    }
    cout<<addr.toIpPort()<<endl;
    TcpStreamPtr stream(TcpStream::connect(addr));
    std::vector<char> mes(opt.length,'S');
    if(!stream->sendAll(mes.data(),mes.size()))
    {
        std::cout<< "cannot send all mes"<<std::endl;
    }
    stream->receiveAll(mes.data(),opt.length);
    return;
}

//g++  -Wall -Wextra -g -O2 InetAddress.cc Socket.cc Acceptor.cc TcpStream.cc echo.cc  -o echo  -std=c++11 
int main(int argc,char** argv)
{
    int  ch;
    option opt;
    // type=string(argv[1]);
    opterr=0;
    while ((ch=getopt(argc,argv,"rtp:h:l:"))!=-1)
    {
        /* code */
        switch (ch)
        {
        case 'r':
            /* code */
            opt.receive=true;
            break;
        case 't':
            opt.transmit=true;
            break;
        case 'p':
            opt.port=atoi(optarg);
            cout<<opt.port<<endl;
            break;
        case 'h':
            opt.hostname=std::string(optarg);
            cout<<opt.hostname<<endl;
            break;
        case 'l':
            opt.length=atoi(optarg);
            cout<<opt.length<<endl;
            break;
        default:
            break;
        }
    }
    
    if(opt.receive==opt.transmit)
    {
        std::cout<<" receive and transmit must different";
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