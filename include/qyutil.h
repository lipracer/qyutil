#ifndef _QYUTIL_H_
#define _QYUTIL_H_

#include <thread>
#include <list>
#include <functional>
#include <condition_variable>
#include <memory>
#include <list>

#include "qylog.h"

#ifndef __ANDROID__
#define __PLATFORM__ (0)
#else
#define __PLATFORM__ (1)
#endif

using namespace std;
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
        for(int i = 0; i < nThread; ++i)
        {
            __th[i] = make_shared<std::thread>(&qyutil::run, this);
            __th[i]->detach();
        }
    }
    ~qyutil()
    {
            // delete [] __th;
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
    shared_ptr<std::thread>  __th[nThread];
    list<QYUtilTask> __msg_queue;
    condition_variable __cvar;
    mutex __cvmtx;
}; 


int NetworkDiagnosis(string HostName, string dnsServer, int timeout);

int _GetHostNameByServer(string HostName, int timeout, string dnsServer, list<string>& ipList);
int _Ping(string host, int times, int package_size, int interval/*S*/, int timeout/*S*/, string& result);
int _TraceRouter(string host, string& result);
}

#endif