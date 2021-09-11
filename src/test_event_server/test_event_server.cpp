#include <string.h>
#ifndef _WIN32 
#include <signal.h>
#endif 
#include <event2/event.h>
#include <event2/listener.h>
#include <iostream>
using namespace std;
#define SPORT 5002

int main()
{
#ifdef _WIN32
    //初始化socket库
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#else 
    //忽略管道信号，发送数据给已关闭的socket
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        return 1;
#endif
    std::cout << "test event server!\n";
    //创建libevent的上下文
    event_base* base = event_base_new();
    
	evutil_socket_t sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock <= 0){
		cout << "socket error: " << strerror(errno) << endl;
		return -1;
	}

    
    //事件分发处理
    event_base_dispatch(base);
    event_base_free(base);

#ifdef _WIN32
    WSACleanup();
    system("pause");
#endif
    return 0;
}

