#include <iostream>
#include <boost/asio.hpp>
#include <cstring>
#include <chrono>

using namespace boost;
const int MAXLENGTH = 1024 * 2;
const int HEAD_LENGTH = 2;

int main(){
    try
    {
        boost::asio::io_context ioc;
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 9999);
        asio::ip::tcp::socket sock(ioc);
        boost::system::error_code ec;
        sock.connect(ep, ec);
        if(ec == asio::error::host_not_found){
            std::cout << "connect error" << "code is:" << ec.value() << std::endl;
            return 0;
        }   

        std::thread sendThread([&sock]{
            for(;;){
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                // std::cout << "Enter message:";
                // char request[MAXLENGTH];
                // std::cin.getline(request, MAXLENGTH);
                const char *request = "hello world";
                std::size_t reqLen = strlen(request);
                char sendData[MAXLENGTH] = {0};
                memcpy(sendData, &reqLen, HEAD_LENGTH);
                memcpy(sendData + 2, request, reqLen);
                boost::asio::write(sock, asio::buffer(sendData, reqLen + HEAD_LENGTH));
            }
        });

        std::thread recvThread([&sock]{
            for(;;){
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                char replayHead[HEAD_LENGTH];
                std::size_t replayLen = boost::asio::read(sock, asio::buffer(replayHead, HEAD_LENGTH));
                short msgLen = 0;
                memcpy(&msgLen, replayHead, HEAD_LENGTH);
                char msg[MAXLENGTH] = {0};
                std::size_t msgLength = boost::asio::read(sock, asio::buffer(msg, msgLen));

                std::cout << "replay is :" << msg << std::endl; 
                std::cout << "replay len is:" << msgLen << std::endl;
            }
        });

        sendThread.join();
        recvThread.join();
        
       
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
    
}