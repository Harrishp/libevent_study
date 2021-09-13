#include "XFtpServerCMD.h"
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <string.h>
using namespace std;

void XFtpServerCMD::Event(struct bufferevent *bev,short what){
  //如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
  if(what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)){
    cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
    bufferevent_free(bev);
    delete this;
  }
}

//子线程XTread event事件分发
void XFtpServerCMD::Read(struct bufferevent *bev){
  char data[1024] = {0};
  for(;;){
    int len = bufferevent_read(bev,data,sizeof(data)-1);
    if(len < 0) break;
    data[len] = '\0';
    cout << "Recv CMD:" << data << flush;

    if(strstr(data,"quit")){
      bufferevent_free(bev);
      delete this;
      break;
    }
  }
}

//初始化任务 运行在子线程中
bool XFtpServerCMD::Init(){
  cout << "XFtpServerCMD::Init()" << endl;
  //监听socket bufferevent
  //base socket
  bufferevent *bev = bufferevent_socket_new(base,sock,BEV_OPT_CLOSE_ON_FREE);

  this->SetCallback(bev);

  //添加超时
  timeval rt = {60,0};
  bufferevent_set_timeouts(bev,&rt,0);
  return true;
}

XFtpServerCMD::XFtpServerCMD(){}
XFtpServerCMD::~XFtpServerCMD(){}
