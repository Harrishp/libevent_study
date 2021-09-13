#include "XFtpTask.h"
#include <event2/event.h>
#include <event2/bufferevent.h>
using namespace std;

//回复cmd消息
void XFtpTask::ResCMD(string msg){
    if(!cmdbev) return;
    bufferevent_write(cmdbev,msg.c_str(),msg.size());
}

void XFtpTask::SetCallback(struct bufferevent *bev){
    bufferevent_setcb(bev,ReadCB,WriteCB,EventCB,this);
    bufferevent_enable(bev,EV_READ|EV_WRITE);
}

void XFtpTask::ReadCB(bufferevent *bev,void *arg){
    XFtpTask *t = (XFtpTask *)arg;
    t->Read(bev);
}

void XFtpTask::WriteCB(bufferevent *bev,void *arg){
    XFtpTask *t = (XFtpTask *)arg;
    t->Write(bev);
}

void XFtpTask::EventCB(struct bufferevent *bev,short what,void *arg){
    XFtpTask *t = (XFtpTask *)arg;
    t->Event(bev,what);
}
