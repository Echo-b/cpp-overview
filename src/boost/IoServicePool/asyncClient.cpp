#include <iostream>
#include <boost/asio.hpp>
#include <jsoncpp/json/json.h>
#include <cstring>
#include <chrono>
#include <vector>
#include <thread>
#include "const.h"

std::vector<std::thread> threads;

using namespace boost;


int main(){
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 100; ++i){
        threads.emplace_back([](){
            try
            {
                boost::asio::io_context ioc;
                asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 9999);
                asio::ip::tcp::socket sock(ioc);
                boost::system::error_code ec;
                sock.connect(ep, ec);
                if(ec == asio::error::host_not_found){
                    std::cout << "connect error" << "code is:" << ec.value() << std::endl;
                    return;
                }   

                int j = 0; 
                while(j < 500){
                    Json::Value root;
                    root["id"] = 1001;
                    root["data"] = "hello world";
                    std::string request = root.toStyledString();
                    std::size_t reqLen = request.length();
                    char sendData[MAX_LENGTH] = {0};

                    short msgid = 1001;
                    short msgIdHost = boost::asio::detail::socket_ops::host_to_network_short(msgid);
                    memcpy(sendData, &msgIdHost, HEAD_ID_LEN);

                    short tLen = asio::detail::socket_ops::host_to_network_short(reqLen);
                    memcpy(sendData + HEAD_ID_LEN, &tLen, HEAD_DATA_LEN);
                    memcpy(sendData + HEAD_ID_LEN + HEAD_DATA_LEN, request.c_str(), reqLen);
                    boost::asio::write(sock, asio::buffer(sendData, reqLen + HEAD_TOTAL_LEN));


                    std::cout << "begin to receive: " << std::endl;
                    char replayHead[HEAD_TOTAL_LEN];
                    std::size_t replayLen = boost::asio::read(sock, asio::buffer(replayHead, HEAD_TOTAL_LEN));
                    short _msgid = 0;
                    memcpy(&_msgid, replayHead, HEAD_ID_LEN);
                    _msgid = asio::detail::socket_ops::network_to_host_short(_msgid);

                    short _msgLen = 0;
                    memcpy(&_msgLen, replayHead + HEAD_ID_LEN, HEAD_DATA_LEN);
                    _msgLen = asio::detail::socket_ops::network_to_host_short(_msgLen);

                    char msg[MAX_LENGTH] = {0};
                    std::size_t msgLength = boost::asio::read(sock, asio::buffer(msg, _msgLen));

                    Json::Reader read;
                    // Json::Value root;
                    read.parse(std::string(msg, msgLength), root);

                    std::cout << "replay msg is :" << root["data"].asString() << std::endl; 
                    std::cout << "replay id is:" << root["id"].asInt() << std::endl;
                    ++j;
                }      
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for(auto &t : threads){
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "spend time: " << duration.count() << std::endl;

    
    return 0;
    
}