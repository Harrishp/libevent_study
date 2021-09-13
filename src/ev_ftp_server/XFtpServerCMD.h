#pragma once 
#include "XFtpTask.h"
#include <map>

class XFtpServerCMD:public XFtpTask{
public:
    //初始化任务
    virtual bool Init();
    virtual void Read(struct bufferevent *bev);
    virtual void Event(struct bufferevent *bev,short what);
    //注册命令处理对象，不需要考虑线程安全，调用时还未分发到线程
    void Reg(std::string,XFtpTask *call);

    XFtpServerCMD();
    ~XFtpServerCMD();
private:
    std::map<std::string,XFtpTask*>calls;
};
