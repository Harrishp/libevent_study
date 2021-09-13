#include "XThreadPool.h"
#include "XThread.h"
#include <iostream>
#include <thread>
#include <unistd.h>
using namespace std;

//分发线程
void XThreadPool::Dispatch(XTask *task){
  //轮询
  if(!task) return;
  int tid = (lastThread + 1) % threadCount;
  lastThread = tid;
  XThread *t = threads[tid];
  t->AddTask(task);

  //激活线程
  t->Activate();
}

//初始化所有线程并启动线程
void XThreadPool::Init(int threadCount){
  this->threadCount = threadCount;
  this->lastThread = -1;
  for(int i = 0; i < threadCount; i++){
    XThread* t = new XThread();
    t->id = i + 1;
    cout << "create thread:" << i << endl;
    t->Start();
    threads.push_back(t);
    //this_thread::sleep_for(2ms); //clang中好像不支持
    sleep(1);
  }
}

