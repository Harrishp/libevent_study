#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#ifndef _WIN32 
#include <signal.h>
#include <string.h>
#else 
#endif
using namespace std;
#define SPORT 5001

bufferevent_filter_result filter_in(evbuffer *s,evbuffer *d, ev_ssize_t limit,bufferevent_flush_mode mode,void *arg){
  cout << "filter_in" << endl;
  char data[1024] = {0};
  //读取并清理原数据
  int len = evbuffer_remove(s,data,sizeof(data)-1);

  //转成大写字母
  for(int i = 0; i < len; ++i){
    data[i] = toupper(data[i]);
  }
  evbuffer_add(d,data,len);
  return BEV_OK;
}

bufferevent_filter_result filter_out(evbuffer *s,evbuffer *d, ev_ssize_t limit,bufferevent_flush_mode mode,void *arg){
  cout << "filter_out" << endl;
  char data[1024] = {0};
  //读取并清理原数据
  int len = evbuffer_remove(s,data,sizeof(data)-1);
  string str = "";
  str += "=================\n";
  str += data;
  str += "=================\n";
  evbuffer_add(d,str.c_str(),str.size());
  return BEV_OK;
}
//错误，超时(连接断开会进入)
void event_cb(bufferevent* be,short events,void* arg){
  cout << "event_cb" << endl;
}

void write_cb(bufferevent* be,void* arg){
  cout << "write_cb" << endl;
}

void read_cb(bufferevent* be,void* arg){
  cout << "read_cb" << endl;
  char data[1024] = {0};
  int len = bufferevent_read(be,data,sizeof(data)-1);
  cout << data << endl;
  //回复客户消息，经过输出过滤
  bufferevent_write(be,data,len);
}

void listen_cb(evconnlistener *ev,evutil_socket_t s,sockaddr* sin,int slen,void* arg){
  cout << "listen_cb" << endl;
  event_base* base = (event_base*)arg;

  //创建bufferevent上下文 
  //BEV_OPT_CLOSE_ON_FREE清理bufferevent时关闭socket
  bufferevent *bev = bufferevent_socket_new(base,s,BEV_OPT_CLOSE_ON_FREE);

  //绑定bufferevent filter
  bufferevent *bev_filter = bufferevent_filter_new(bev,
      filter_in, //输入过滤函数
      filter_out, //输出过滤函数
      BEV_OPT_CLOSE_ON_FREE, //关闭filter的同时关闭bufferevent
      0,  //清理的回调函数
      0   //传递给回调的参数
      );

  //设置回调函数
  bufferevent_setcb(bev_filter,read_cb,write_cb,event_cb,NULL);
  //添加监控事件
  bufferevent_enable(bev_filter,EV_READ|EV_WRITE);
}

int main(int argc,char* argv[]){
#ifdef _WIN32 
  //初始化socket库
  WSADATA wsa;
  WSAStartup(MAKEWORD(2,2),&wsa);
#else 
  if(signal(SIGPIPE,SIG_IGN) == SIG_ERR)
    return 1;
#endif 

  event_base* base = event_base_new();

  cout << "test buffer" << endl;

  //设置监听的端口和地址
  sockaddr_in sin;
  memset(&sin,0,sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(SPORT);
  
  evconnlistener *ev = evconnlistener_new_bind(base,
      listen_cb,
      base,
      LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,
      10,
      (sockaddr*)&sin,
      sizeof(sin)
      );

  //进入事件主循环
  event_base_dispatch(base);
  evconnlistener_free(ev);
  event_base_free(base);
#ifdef _WIN32 
  WSACleanup();
#endif 
  return 0;
}
