#include <iostream>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#ifndef _WIN32 
#include <signal.h>
#include <string.h>
#else 
#endif
using namespace std;

#define SPORT 5001

static string recvstr = "";
static int recvCount = 0;
static int sendCount = 0;

//错误，超时(连接断开会进入)
void event_cb(bufferevent* be,short events,void* arg){
  cout << "[E]" << endl;
  //读取超时时间发生后，数据读取停止
  if(events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING){
    cout << "BEV_EVENT_READING BEV_EVENT_TIMEOUT" << endl;
    bufferevent_free(be);
  }else if(events & BEV_EVENT_ERROR){
    bufferevent_free(be);
  }else{
    cout << "others" << endl;
  }
}

void write_cb(bufferevent* be,void* arg){
  cout << "[W]" << endl;
}

void read_cb(bufferevent* be,void* arg){
  cout << "[R]" << endl;
  char data[1024] = {0};
  //读取输入缓冲数据
  int len = bufferevent_read(be,data,sizeof(data)-1);
  if(len <= 0) return ;
  if(strstr(data,"quit") != NULL){
    cout << "quit";
    //退出并关闭socket
    bufferevent_free(be);
  }
  //发送数据写入到输出缓冲
  bufferevent_write(be,"ok",3);
}

void listen_cb(evconnlistener *ev,evutil_socket_t s,sockaddr* sin,int slen,void* arg){
  cout << "listen_cb" << endl;
  event_base* base = (event_base*)arg;

  //创建bufferevent上下文 
  //BEV_OPT_CLOSE_ON_FREE清理bufferevent时关闭socket
  bufferevent *bev = bufferevent_socket_new(base,s,BEV_OPT_CLOSE_ON_FREE);

  //添加监控事件
  bufferevent_enable(bev,EV_READ|EV_WRITE);

  //设置水位
  bufferevent_setwatermark(bev,EV_READ,5,10);
  bufferevent_setwatermark(bev,EV_WRITE,5,0);

  //超时时间设置
  timeval t1 = {3,0};
  //bufferevent_set_timeouts(bev,&t1,0);

  //设置回调函数
  bufferevent_setcb(bev,read_cb,write_cb,event_cb,base);
}

//错误，超时(连接断开会进入)
void client_event_cb(bufferevent* be,short events,void* arg){
  cout << "[client_E]" << endl;
  //读取超时时间发生后，数据读取停止
  if(events & BEV_EVENT_TIMEOUT && events & BEV_EVENT_READING){
    cout << "BEV_EVENT_READING BEV_EVENT_TIMEOUT" << endl;
    bufferevent_free(be);
    return;
  }else if(events & BEV_EVENT_ERROR){
    bufferevent_free(be);
    return;
  }

  //服务端的关闭事件
  if(events & BEV_EVENT_EOF){
    cout << "BEV_EVENT_EOF" << endl;
    bufferevent_free(be);
  }
  if(events & BEV_EVENT_CONNECTED){
    cout << "BEV_EVENT_CONNECTED" << endl;
    //触发write
    bufferevent_trigger(be,EV_WRITE,0);
  }
}

void client_write_cb(bufferevent *be,void *arg){
  cout << "[client_W]" << endl;
  FILE *fp = (FILE *)arg;
  char data[1024] = {0};
  int len = fread(data,1,sizeof(data)-1,fp);
  if(len <= 0){
    //读到结尾或者文件出错
    fclose(fp);
    //立刻清理，可能会造成缓冲数据没有发送结束
    //bufferevent_free(be);
    bufferevent_disable(be,EV_WRITE);
    return;
  }
  sendCount += len;
  //写入buffer
  bufferevent_write(be,data,len);
}

void client_read_cb(bufferevent *be,void *arg){
  cout << "[client_R]" << endl;
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

  {
    //调用客户端代码
    //-1内部创建socket
    bufferevent *bev = bufferevent_socket_new(base,-1,BEV_OPT_CLOSE_ON_FREE);
    sockaddr_in sin;
    memset(&sin,0,sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(5001);
    evutil_inet_pton(AF_INET,"127.0.0.1",&sin.sin_addr.s_addr);
    
    FILE *fp = fopen("test_buffer_client.cpp","rb");
    //设置回调函数
    bufferevent_setcb(bev,client_read_cb,client_write_cb,client_event_cb,fp);
    bufferevent_enable(bev,EV_READ|EV_WRITE);
    int ret = bufferevent_socket_connect(bev,(sockaddr*)&sin,sizeof(sin));
    if(ret == 0){
      cout << "connected" <<endl;
    }
  }

  //进入事件主循环
  event_base_dispatch(base);
  evconnlistener_free(ev);
  event_base_free(base);
  return 0;
}
