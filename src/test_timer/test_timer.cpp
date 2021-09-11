#include <iostream>
#include <event2/event.h>
#ifndef _WIN32
#include <signal.h>
#endif
using namespace std;
static timeval t1= {1,0};
void timer1(int sock,short what,void* arg){
    cout << "[timer1]" << flush;
    event* ev = (event*)arg;
    //no pending 
    if(evtimer_pending(ev,&t1)){
        evtimer_del(ev);
        evtimer_add(ev,&t1);
    }
}

void timer2(int sock,short what,void* arg){
    cout << "[timer2]" << flush;
}

void timer3(int sock, short what,void* arg){
    cout << "[timer3]" << flush;
}

int main(int argc, char *argv[]){
#ifdef _WIN32
    WSADATA sa;
    WSAStartup(MAKEWORD(2,2),&sa);
#else
    if(signal(SIGPIPE,SIG_IGN) == SIG_ERR)
        return 1;
#endif 
    event_base* base = event_base_new();
    //定时器
    cout << "test timer" << endl;

    //非持久
    event* ev1 = evtimer_new(base,timer1,event_self_cbarg());
    if(!ev1){
        cerr << "timer1 evtimer_new failed!" << endl;
        return -1;
    }
    evtimer_add(ev1,&t1); //插入性能O(logn)

    struct timeval t2;
    t2.tv_sec = 1;
    t2.tv_usec = 200000; //微秒
    event* ev2 = event_new(base,-1,EV_PERSIST,timer2,0);
    evtimer_add(ev2,&t2);

    //超时优化性能优化，默认event用二叉堆存储(完全二叉树)，插入删除O(logn)
    //优化到双向队列  插入删除O(1)
    struct timeval tv_in = {3,0};
    const timeval  *t3;
    t3 = event_base_init_common_timeout(base,&tv_in);
    
    event* ev3 = event_new(base,-1,EV_PERSIST,timer3,0);
    evtimer_add(ev3,t3); //插入性能O(1)

    //进入事件主循环
    event_base_dispatch(base);

    event_base_free(base);
    return 0;
}
