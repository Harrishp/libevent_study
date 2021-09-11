#include <iostream>
#include <event2/event.h>
#include <signal.h>
using namespace std;

void ctrl_c(int sock,short which,void *arg){
    cout << "ctrl_c" << endl;
}

void kill(int sock,short which,void *arg){
    cout << "kill" << endl;
    event *ev = (event *)arg;
    //如果处于非待决状态
    if(!evsignal_pending(ev,NULL)){
        event_del(ev);
        event_add(ev,NULL);
    }
}

int main(int argc, char* argv[]){
    event_base* base = event_base_new();

    //添加ctrl+c信号事件，处于no pending 
    //evsignal_new隐藏的状态EV_SIGNAL|EV_PERSIST
    event* csig = evsignal_new(base,SIGINT,ctrl_c,base);
    if(!csig){
        cerr << "SIGINT evsignal_new failed" << endl;
        return -1;
    }

    //添加事件到pending
    if(event_add(csig,0) != 0){
        cerr << "SIGINT event_add failed" << endl;
        return -1;
    }

    //添加kill信号
    //非持久事件，只进入一次event_self_cbarg()传递当前的event
    event* ksig = event_new(base,SIGTERM,EV_SIGNAL,kill,event_self_cbarg());
    if(!ksig){
        cerr << "SIGTERM event_new failed" << endl;
        return -1;
    }

    if(event_add(ksig,0) != 0){
        cerr << "SIGTERM event_add failed" << endl;
        return -1;
    }

    //进入事件主循环
    event_base_dispatch(base);
    event_free(csig);
    event_base_free(base);
    return 0;
}
