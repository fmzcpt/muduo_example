#include "Acceptor.h"
#include "TcpStream.h"
#include "InetAddress.h"

#include <time.h>
#include<sys/time.h>
#include <thread>
#include <mutex>
#include <iostream>
#include <string.h>
#include <stdint.h>
using namespace std;
uint64_t g_bytes;
// std::mutex mut;

double now()
{
    timeval tv;
    gettimeofday(&tv,NULL);
    return 1.0*tv.tv_sec+tv.tv_usec/1000000;
}

string  getMessage()
{
    // string line="";
    // for(int i=0;i<1024;i++)
    // {
    //     line.push_back("0123456789abcdef"[i%16]);
    // }
    // return line;
  std::string line;
  for (int i = 33; i < 127; ++i)
  {
    line.push_back(char(i));
  }
  line += line;

  std::string message;
  for (size_t i = 0; i < 127-33; ++i)
  {
    message += line.substr(i, 72) + '\n';
  }
  return message;

}

void measure()
{
    double start=now();
    while(true)
    {
        timespec tv={1,0};
        ::nanosleep(&tv,NULL);
        double end=now();
        // mut.lock();
        // int value=g_bytes;
        // g_bytes=0;
        // mut.unlock();
        int value=__sync_lock_test_and_set(&g_bytes,0);
        double es=end-start;
        start=end;
        if(value)
        {
            cout<<1.0*value/1024/1024/es<<endl;
        }
    }
    return;
}

void chargen(TcpStreamPtr stream)
{  std::string msg=getMessage(); 
    while(true)
    {   
        int nw=stream->sendAll(msg.data(),msg.size());
        if(nw<0)
        {
            break;
        }
        // mut.lock();
        // g_bytes+=nw;
        // mut.unlock();
        __sync_fetch_and_add(&g_bytes, nw);
    }
    return;
}
// g++ Acceptor.cc InetAddress.cc TcpStream.cc chargen.cc Socket.cc -o chargen -std=c++11 -lpthread
int main(int argc,char** argv)
{
    
   
    thread th(measure);
    th.detach();

    if(argc <= 1)
    {
        cout<<"-l+port or hostname+port"<<endl;
    }
    uint32_t port=atoi(argv[2]);
    if(strcmp(argv[1],"-l")==0)
    {
        InetAddress addr(port);
        Acceptor acp(addr);
        while (TcpStreamPtr stream=acp.accept())
        {
            /* code */
            thread thr(chargen,move(stream));
            thr.detach();
        }
        
    }else
    {
        InetAddress addr(port);
        if(!InetAddress::resolve(argv[1],&addr))
        {
            perror("cannot reslove the localhost");
        }
        TcpStreamPtr stream(TcpStream::connect(addr));
        if(stream)
        {
            thread thr(chargen,move(stream));
            thr.detach();
        }
    }
    return 0;      
}