#ifndef _QYUTIL_H_
#define _QYUTIL_H_

#include <thread>
#include <list>
#include <functional>
#include <condition_variable>
#include "qylog.h"

namespace QyUtil
{
using QYUtilTask = std::pair<function<int(void)>, function<int(void)>>;

template<int nThread=1>//线程数
class qyutil
{
public:
    static qyutil& getInstance()
    {
        static qyutil<nThread> __instance;
        return __instance;
    }
public:
    qyutil(/* args */)
    {
        LOGI("construct");
        __th = new thread(&qyutil::run, this);
        __th->detach();
    }
    ~qyutil()
    {
            delete __th;
    }
    void run()
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
                if(tmp_task.second) tmp_task.second();
            }
        }
    }
    void put_task(QYUtilTask task)
    {
        lock_guard<mutex> tMtx(__cvmtx);
        __msg_queue.push_back(task);
        __cvar.notify_one();
    }
private:
    thread* __th;
    list<QYUtilTask> __msg_queue;
    condition_variable __cvar;
    mutex __cvmtx;
}; 
}

int NetworkDiagnosis(string HostName, string dnsServer);

#endif
