#include "Acceptor.h"
#include "TcpStream.h"
#include "InetAddress.h"

#include <iostream>
#include <string.h>
#include <thread>
#include <unistd.h>
using namespace std;

int write_n(int fd, const void* buf, int length)
{
  int written = 0;
  while (written < length)
  {
    int nw = ::write(fd, static_cast<const char*>(buf) + written, length - written);
    if (nw > 0)
    {
      written += nw;
    }
    else if (nw == 0)
    {
      break;  // EOF
    }
    else if (errno != EINTR)
    {
      perror("write");
      break;
    }
  }
  return written;
}
void run(TcpStreamPtr stream)
{
    thread thr([&stream](){
        char buf[8192];
        int nr=0;
        while((nr=stream->receiveSome(buf,sizeof buf))>0)
        {
            int nw=write_n(STDOUT_FILENO,buf,nr);
            if(nw<nr)
            {
                break;
            }
        }
        ::exit(0);
    });
    // char buf[8192];
    // int nr=0;
    // while((nr=::read(STDIN_FILENO,buf,sizeof buf))>0)
    // {
 
    //     int nw=stream->sendAll(buf,nr);
    //     if(nw<nr)
    //     {
    //         break;
    //     }
    // }

  char buf[8192];
  int nr = 0;
  while ( (nr = ::read(STDIN_FILENO, buf, sizeof(buf))) > 0)
  {

    int nw = stream->sendAll(buf, nr);
    if (nw < nr)
    {
      break;
    }
  }
    stream->shutdown();
    thr.join();

}

int main(int argc,char** argv)
{
    if(argc<1)
    {
        cout<<" -l or hostname + port"<<endl;
        return 0;
    }

    uint32_t port=atoi(argv[2]);
    if(strcmp("-l",argv[1])==0)
    {
        InetAddress addr(port);
        Acceptor acp(addr);
        TcpStreamPtr stream(acp.accept());
        if(stream)
        {

            run(move(stream));
            
        }
    }else
    {
        InetAddress addr(port);
        if(!InetAddress::resolve(argv[1],&addr))
        {
            perror("cannot reslove");
        }
        TcpStreamPtr stream(TcpStream::connect(addr));
        if(stream)
        {
            // char buf[8192]
            run(move(stream));
        }
        
    }
    
}