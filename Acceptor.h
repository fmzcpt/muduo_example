#include "InetAddress.h"
#include "Socket.h"
#include"TcpStream.h"
#include <memory>
class InetAddress;
class TcpStream;

typedef std::unique_ptr<TcpStream> TcpStreamPtr;

class Acceptor
{
    public:
        Acceptor(const InetAddress&);
        ~Acceptor()=default;
        Acceptor(Acceptor&&)=default;
        Acceptor& operator=(Acceptor&&)=default;


        TcpStreamPtr accept();
    
    private:
        Socket sock_;
};