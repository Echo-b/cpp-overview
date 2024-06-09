#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

int main()
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1)
    {
        perror("socket");
        exit(0);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);   // 大端端口
    addr.sin_addr.s_addr = INADDR_ANY;  
    int ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }

    ret = listen(lfd, 128);
    if(ret == -1)
    {
        perror("listen");
        exit(0);
    }

    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);
    if(cfd == -1)
    {
        perror("accept");
        exit(0);
    }
   
    char ip[24] = {0};
    std::cout << "客户端的IP地址: " <<  inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)) << "端口: " << ntohs(cliaddr.sin_port) << std::endl;

   
    while(1)
    {
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        int len = read(cfd, buf, sizeof(buf));
        if(len > 0)
        {
            std::cout << "客户端say:" << buf << std::endl;
            write(cfd, buf, len);
        }
        else if(len  == 0)
        {
            std::cout << "客户端断开了连接..." << std::endl;
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }

    close(cfd);
    close(lfd);

    return 0;
}

