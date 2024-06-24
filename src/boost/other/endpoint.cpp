#include "endpoint.h"

int clientEndPoint(){
    
    std::string ip = "192.168.221.128";
    uint32_t port = 9999;
    system::error_code ec;
    asio::ip::address addr = asio::ip::address::from_string(ip, ec);
    if(ec.value() != 0){
        std::cout << "parse ip error err code is: " << ec.value() << " err msg is :" << ec.message() << std::endl;
        return ec.value();  
    }
    asio::ip::tcp::endpoint ep(addr, port);
    return 0;

}

int serverEndPoint(){
    asio::ip::address addr = asio::ip::address_v4::any();
    uint32_t port = 9999;
    asio::ip::tcp::endpoint ep(addr, port);
    return 0;
}

int createSocket(){
    asio::io_context ios;
    asio::ip::tcp protocol = asio::ip::tcp::v4();
    asio::ip::tcp::socket sock(ios);

    boost::system::error_code ec;

    sock.open(protocol, ec);

    if(ec.value() != 0){
        std::cout << "parse ip error err code is: " << ec.value() << " err msg is :" << ec.message() << std::endl;
        return ec.value(); 
    }

    return 0;
    
}

int createAcceptorSocket(){
    asio::io_context ios;
    asio::ip::tcp protocol = asio::ip::tcp::v4();
    asio::ip::tcp::acceptor acceptor(ios);

    boost::system::error_code ec;

    acceptor.open(protocol, ec);

    if(ec.value() != 0){
        std::cout << "parse ip error err code is: " << ec.value() << " err msg is :" << ec.message() << std::endl;
        return ec.value(); 
    }

    return 0;

}

int bindAcceptorSocket(){
    unsigned short port = 9999;
    asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port);

    asio::io_context ios;

    asio::ip::tcp::acceptor acceptor(ios);

    boost::system::error_code ec;
    acceptor.bind(ep, ec);

    if(ec.value() != 0){
        std::cout << "parse ip error err code is: " << ec.value() << " err msg is :" << ec.message() << std::endl;
        return ec.value(); 
    }

    return 0;
}

int connectToEnd(){
    std::string ip = "192.168.221.128";
    uint32_t port = 9999;
    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip), port);

        asio::io_context ios;

        asio::ip::tcp::socket sock(ios);

        sock.connect(ep);
    }
    catch(system::system_error& e)
    {
        std::cerr << e.what() << '\n';
        return e.code().value();
    }
    return 0;
    
}

int acceptNewConnection(){
    int BACKLOG_SIZE = 30;
    unsigned short port = 9999;

    asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port);

    asio::io_context ios;

    try
    {
        asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

        // boost::system::error_code ec;
        acceptor.bind(ep);

        acceptor.listen(BACKLOG_SIZE);

        asio::ip::tcp::socket sock(ios);
        acceptor.accept(sock);
    }
    catch(system::system_error& e)
    {
        std::cerr << e.what() << '\n';
        return e.code().value();
    }
    return 0;

}

void writeToSocket(asio::ip::tcp::socket &sock){
    std::string buf = "hello world";
    std::size_t totalBytesWritten = 0;

    while(totalBytesWritten != buf.length()){
        totalBytesWritten += sock.write_some(asio::buffer(
            buf.c_str() + totalBytesWritten, buf.length() - totalBytesWritten));
    }

}

int sendDataBySendFunc(){
    std::string ip = "192.168.221.12";
    unsigned short port = 9999;

    try
    {
        //create endpoint
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip), port);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        
        std::string buf = "hello world!";
        int sendLen = sock.send(asio::buffer(buf.c_str(), buf.length()));
        if(sendLen <= 0 ){
            std::cout << "send failed" << std::endl;
            return 0;
        }
    }
    catch(const system::system_error& e)
    {
        std::cerr << e.what() << '\n';
        return e.code().value();
    }
    return 0;

}

int sendDataByWriteFunc(){
    std::string ip = "192.168.221.12";
    unsigned short port = 9999;

    try
    {
        //create endpoint
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip), port);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        
        std::string buf = "hello world!";
        int sendLen = asio::write(sock, asio::buffer(buf.c_str(), buf.length()));
        if(sendLen <= 0 ){
            std::cout << "send failed" << std::endl;
            return 0;
        }
    }
    catch(const system::system_error& e)
    {
        std::cerr << e.what() << '\n';
        return e.code().value();
    }
    return 0;

}

std::string readFromSocket(asio::ip::tcp::socket &sock){
    const unsigned char MESSAGE_SIZE = 7;
    char buf[MESSAGE_SIZE];
    std::size_t totalBytesRead = 0;

    while(totalBytesRead != MESSAGE_SIZE){
        totalBytesRead += sock.read_some(asio::buffer(buf + totalBytesRead, MESSAGE_SIZE - totalBytesRead));
    }
    return std::string(buf, totalBytesRead);
}

int readDataByReadSome(){
    std::string ip = "192.168.221.112";
    unsigned short port = 9999;
    try{
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip), port);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        readFromSocket(sock);
    }
    catch(system::system_error& e){
        std::cerr << e.what() << '\n';
        return e.code().value();
    }
    return 0;
}

int readDataByReceiveFunc(){
    std::string ip = "192.168.221.112";
    unsigned short port = 9999;
    try{
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip), port);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);

        const unsigned char BUFF_SIZE = 7;
        char buf[BUFF_SIZE];
        int receiveLen = sock.receive(asio::buffer(buf,BUFF_SIZE));

        if(receiveLen <= 0){
            std::cout << "receive failed" << std::endl;
            return 0;
        }
    }
    catch(system::system_error& e){
        std::cerr << e.what() << '\n';
        return e.code().value();
    }
    return 0;
}

int readDataByReadFunc(){
    std::string ip = "192.168.221.112";
    unsigned short port = 9999;

    try
    {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip), port);
        asio::io_context ioc;
        asio::ip::tcp::socket sock(ioc, ep.protocol());
        sock.connect(ep);
        const unsigned char BUFF_SIZE = 7;
        char buf[BUFF_SIZE];
        int receiveLen = asio::read(sock, asio::buffer(buf, BUFF_SIZE));
        if(receiveLen <= 0){
            std::cout << "receive failed" << std::endl;
            return 0;
        }
    }
    catch(system::system_error& e)
    {
        std::cerr << e.what() << '\n';
        return e.code().value();
    }
    return 0;
    
}

std::string readDataByUntil(asio::ip::tcp::socket &sock){
    asio::streambuf buf;
    asio::read_until(sock, buf, '\n');
    std::string msg;
    std::istream istream(&buf);
    std::getline(istream, msg);
    return msg;
}