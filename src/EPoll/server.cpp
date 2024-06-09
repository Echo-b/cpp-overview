#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

pthread_mutex_t mutex;

struct sockInfo {
    int fd;
    int epfd;
};


void errorHandle(std::string s){
    std::cout << s << std::endl;
    exit(0);
}


void* connetFunc(void *arg){

    sockInfo *info = (sockInfo *) arg;

    int cfd = accept(info->fd, NULL, NULL);
    if(-1 == cfd) {
        delete info;
        errorHandle("accept error");
    }

    struct epoll_event ev;
    ev.data.fd = cfd;
    ev.events = EPOLLIN;
    pthread_mutex_lock(&mutex);
    int ret = epoll_ctl(info->epfd, EPOLL_CTL_ADD, cfd, &ev);
    pthread_mutex_unlock(&mutex);
    if(ret == -1){
        delete info;
        errorHandle("epoll ctl add error");
    } 
    delete info;
    return nullptr;

}

void *commucationFunc(void *arg){
    sockInfo *info = (sockInfo *) arg;
    int curfd = info->fd;
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    int len = recv(curfd, buf, sizeof(buf), 0);
    if(0 == len){
        std::cout << "client quit..." << std::endl;
        pthread_mutex_lock(&mutex);
        int ret = epoll_ctl(info->epfd, EPOLL_CTL_DEL, curfd, NULL);
        pthread_mutex_unlock(&mutex);
        if(-1 == ret){
            delete info;
            errorHandle("epoll ctl del error" + std::to_string(errno));
        }
        close(curfd);
    }else if(len > 0){
        std::cout << "client say: " << buf << "ID:"  << std::to_string(pthread_self()) << std::endl;
        send(curfd, buf,len, 0);
    }else {
        delete info;
        errorHandle("recv error");
    }
    delete info;
    return nullptr;
}

void server(){
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1) errorHandle("scoket error");
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(9999);
    serv_addr.sin_family = AF_INET;


    int ret = bind(lfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(ret == -1) errorHandle("bing error");
    ret = listen(lfd, 128);
    if(ret == -1) errorHandle("listen error");

    int epfd = epoll_create(100);
    if(-1 == epfd) errorHandle("epoll create error");

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = lfd;

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if(ret == -1) errorHandle("epoll ctl add error");

    struct epoll_event evs[1024];
    int size = sizeof(evs) / sizeof(struct epoll_event);
    ret = pthread_mutex_init(&mutex, NULL);
    if(ret == -1) errorHandle("mutex init error");
    while (true)
    {
        pthread_mutex_lock(&mutex);
        int num = epoll_wait(epfd, evs, size, -1);
        pthread_mutex_unlock(&mutex);
        for(int i = 0; i < num; ++i){
            // pthread_mutex_lock(&mutex);
            int curfd = evs[i].data.fd;
            // pthread_mutex_unlock(&mutex);
            if(curfd == lfd){
                // create new link
                sockInfo *info = new sockInfo;
                info->fd = lfd;
                info->epfd = epfd;
                pthread_t tid;
                pthread_create(&tid, NULL, connetFunc, info);
                pthread_detach(tid);

            }else{
                // handle data 
                sockInfo *info = new sockInfo;
                info->fd = curfd;
                info->epfd = epfd;
                pthread_t tid;
                pthread_create(&tid, NULL, commucationFunc, info);
                pthread_detach(tid);
            }

        }
    }
    close(lfd);
    pthread_mutex_destroy(&mutex);
}

int main(){
    server();
    return 0;
}