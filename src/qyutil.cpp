#include "qyutil.h"
#include <chrono>
#include <iostream>

#include "qylog.h"

using namespace QyUtil;
using namespace std;

qyutil::qyutil(/* args */)
{
    LOGI("construct");
    __th = new thread(&qyutil::run, this);
    __th->detach();
}

qyutil::~qyutil()
{
    delete __th;
}

void qyutil::put_task(QYUtilTask& task)
{
    lock_guard<mutex> tMtx(__cvmtx);
    __msg_queue.push_back(task);
    __cvar.notify_one();
}

void qyutil::run()
{
    while (true)
    {
        unique_lock<mutex> unlock(__cvmtx);
        if(__msg_queue.empty())
        {
            //block
            __cvar.wait(unlock, [&](){ return !__msg_queue.empty(); });
        }
        auto tmp_task = *__msg_queue.begin();
        __msg_queue.pop_front();
        unlock.unlock();
        
        if(tmp_task.first)
        {
            tmp_task.first();
            tmp_task.second();
        }
    }
}

