#include "XFtpFactory.h"
#include "XFtpServerCMD.h"
XTask *XFtpFactory::CreateTask(){
    XFtpServerCMD *x = new XFtpServerCMD();
    
    return x;
}

XFtpFactory::XFtpFactory(){

}
