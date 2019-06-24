#ifndef _QYUTIL_H_
#define _QYUTIL_H_

#include <thread>

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
private:
    std::thread* __th;
};


#endif
