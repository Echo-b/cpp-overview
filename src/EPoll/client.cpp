#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>

void errorHandle(std::string s){
    std::cout << s << std::endl;
    exit(0);
}

void client(){

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) errorHandle("socket error");

    struct sockaddr_in cli_addr;
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &cli_addr.sin_addr.s_addr);

    int ret = connect(fd, (struct sockaddr*)&cli_addr, sizeof(cli_addr));
    if(ret == -1) errorHandle("connect error");

    while (true)
    {
        char recvBuf[1024];
        fgets(recvBuf, sizeof(recvBuf), stdin);
        write(fd, recvBuf, sizeof(recvBuf));
        read(fd, recvBuf, sizeof(recvBuf));
        std::cout << "recv buf: " << recvBuf << std::endl;
        sleep(1);
    }
    close(fd);
}

int main(){
    client();
    return 0;
}