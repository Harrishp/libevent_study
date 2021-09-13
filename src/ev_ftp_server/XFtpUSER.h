#pragma once 
#include "XFtpTask.h"
#include <iostream>

class XFtpUSER:public XFtpTask{
public:
    virtual void Parse(std::string type,std::string msg);
    XFtpUSER();
    ~XFtpUSER();
};
