#include"Acceptor.h"
#include "TcpStream.h"
#include "InetAddress.h"
#include<sys/time.h>
#include<iostream>
#include<assert.h>
#include<boost/program_options.hpp>
// #include<ctime>

struct opt
{
    /* data */
    // uint32_t ip;
    uint16_t port;
    std::string hostname;
    int length;
    int nums;
    bool receive;
    bool transmit;
    bool nodelay;
    opt():port(0),length(0),nums(0),receive(0),transmit(0),nodelay(0){}
};


struct SessionMessage
{
    /* data */
    int32_t nums;
    int32_t length;
    
}__attribute__((packed));

struct PayloadMessage
{
    /* data */
    int32_t length;
    char data[0];
};

double now()
{
    timeval t={0,0};
    gettimeofday(&t,NULL);
    return t.tv_sec+t.tv_usec/1000000.0;
}

void transmit(opt option)
{
    InetAddress addr(option.port);
    if(!InetAddress::resolve(option.hostname.c_str(),&addr))
    {
        std::cout<<addr.toIp()<<std::endl;
        perror("transmit resolve");
        return;
    }
    TcpStreamPtr stream(TcpStream::connect(addr));
    if(!stream)
    {
        perror("transmit connent");
        return;
    }
    double start=now();
    SessionMessage session={0,0};
    session.length=htonl(option.length);
    session.nums=htonl(option.nums);
    if(stream->sendAll(&session,sizeof session)!=sizeof session)
    {
        perror("transmit send session");
        return;
    }
    const int payload_len=option.length+sizeof(int32_t);
    PayloadMessage* payload=static_cast<PayloadMessage*>(::malloc(payload_len));
    std::unique_ptr<PayloadMessage,void(*)(void*)> freeit(payload,::free);
    payload->length=htonl(option.length);
    for(int i=0;i<option.length;i++)
    {
        payload->data[i]="0123456789abcdef"[i%16];
    }
    double total_len=1.0*option.length*option.nums/1024/1024;
    for(int i=0;i<option.nums;i++)
    {
        if(stream->sendAll(payload,payload_len)!=payload_len)
        {
            perror("transmit payload");
            return;
        }
        int32_t ack;
        if(stream->receiveAll(&ack,sizeof ack)!=sizeof ack)
        {
            perror("trams ack");
            return;
        }
        ack=ntohl(ack);
        assert(ack==option.length);
        // std::cout<<i<<std::endl;
    }
    double time=now()-start;
    std::cout<<"total:"<<total_len<<"MB "<<"time:"<<time<<" speed:"<<total_len/time<<"MiB/s"<<std::endl;
    return;
    

} 

void receive(opt option)
{
    InetAddress addr(option.port);
    Acceptor ac(addr);
    TcpStreamPtr stream(ac.accept());
    if(!stream)
    {
        perror("ttcp receive");
        abort();
    }
    SessionMessage session={0,0};
    if(stream->receiveAll(&session,sizeof session)!=sizeof session)
    {
        perror("session read");
        abort();
    }

    
    session.nums=ntohl(session.nums);
    session.length=ntohl(session.length);
    double total_len=1.0*session.length*session.nums/1024/1024;
    const int payload_lengh=sizeof (int32_t) +session.length;
    PayloadMessage* payload=static_cast<PayloadMessage*>(::malloc(payload_lengh));//分配内存
    std::unique_ptr<PayloadMessage,void(*)(void*)>freeIt(payload,::free); //资源释放 函数指针

    double start=now();
    for(int i=0;i<session.nums;i++)
    {
         payload->length=0;
        if(stream->receiveAll(&payload->length,sizeof(payload->length))!=sizeof(payload->length))
        {
            perror("receive data length");
            abort();
            return;
        }
        payload->length=ntohl(payload->length);
        if(stream->receiveAll(payload->data,payload->length)!=payload->length)
        {
            perror("receive data");
            return;
        }
        const int32_t ack=htonl(payload->length);
        if(stream->sendAll(&ack,sizeof ack)!=sizeof ack)
        {
            perror("receive ack");
            return;
        }

    }
    double time=now()-start;
    std::cout<<"time:"<<time<<" speed:"<<total_len/time<<"MiB/s"<<std::endl;
    return;
}

bool parseCommandLine(int argc, char* argv[], opt* option)
{
  namespace po = boost::program_options;

  po::options_description desc("Allowed options");
  desc.add_options()
      ("help,h", "Help")
      ("port,p", po::value<uint16_t>(&option->port)->default_value(5001), "TCP port")
      ("length,l", po::value<int>(&option->length)->default_value(65536), "Buffer length")
      ("number,n", po::value<int>(&option->nums)->default_value(8192), "Number of buffers")
      ("trans,t",  po::value<std::string>(&option->hostname), "Transmit")
      ("recv,r", "Receive")
      ("nodelay,D", "set TCP_NODELAY")
      ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  option->transmit = vm.count("trans");
  option->receive = vm.count("recv");
  option->nodelay = vm.count("nodelay");
  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    return false;
  }

  if (option->transmit == option->receive)
  {
    printf("either -t or -r must be specified.\n");
    return false;
  }

  printf("port = %d\n", option->port);
  if (option->transmit)
  {
    printf("buffer length = %d\n", option->length);
    printf("number of buffers = %d\n", option->nums);
  }
  else
  {
    printf("accepting...\n");
  }
  return true;
}


int main(int argc, char* argv[])
{
  opt options;
  if (parseCommandLine(argc, argv, &options))
  {
    if (options.transmit)
    {
      transmit(options);
    }
    else if (options.receive)
    {
      receive(options);
    }
    else
    {
      assert(0);
    }
  }
}


