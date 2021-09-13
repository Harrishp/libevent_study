#pragma once 
#include "XTask.h"

class XFtpTask:public XTask{
public:
    virtual void Read(struct bufferevent *bev){}
    virtual void Write(struct bufferevent *bev){}
    virtual void Event(struct bufferevent *bev,short what){}
    void SetCallback(struct bufferevent *bev);
protected:
    static void ReadCB(bufferevent *bev, void *arg);
    static void WriteCB(bufferevent *bev, void *arg);
    static void EventCB(struct bufferevent *bev,short what,void *arg);
};
