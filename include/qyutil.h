#ifndef _QYUTIL_H_
#define _QYUTIL_H_

#include <thread>
#include <list>
#include <functional>
#include <condition_variable>

namespace QyUtil
{    

using namespace std;

using QYUtilTask = std::pair<function<int(void)>, function<int(void)>>;

class qyutil
{
public:
    static qyutil& getInstance()
    {
        static qyutil __instance;
        return __instance;
    }
public:
    qyutil(/* args */);
    ~qyutil();
    void run();
    void put_task(QYUtilTask& task);
private:
    thread* __th;
    list<QYUtilTask> __msg_queue;
    condition_variable __cvar;
    mutex __cvmtx;
};
    
}


#endif
