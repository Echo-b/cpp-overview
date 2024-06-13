#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <string>

using namespace boost;

int clientEndPoint();
int serverEndPoint();
int createSocket();
int createAcceptorSocket();
int bindAcceptorSocket();
int connectToEnd();
int acceptNewConnection();
void writeToSocket(asio::ip::tcp::socket &sock);
int sendDataBySendFunc();
int sendDataByWriteFunc();
std::string readFromSocket(asio::ip::tcp::socket &sock);
int readDataByReadSome();
int readDataByReceiveFunc();
int readDataByReadFunc();
std::string readDataByUntil(asio::ip::tcp::socket &sock);