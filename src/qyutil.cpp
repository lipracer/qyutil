#include "qyutil.h"
#include <chrono>
#include <iostream>

#include "qylog.h"

#if __ANDROID__
#include <android/log.h>
#endif

using namespace std;

qyutil::qyutil(/* args */)
{
    LOGE("----------------------%s", "construct\n");
    __th = new thread(&qyutil::run, this);
    __th->detach();
}

qyutil::~qyutil()
{
    delete __th;
}

void qyutil::run()
{
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
        //cout << "thread run" << endl;

        LOGE("%s", "thread run\n");

    }
}

//static qyutil __instance;
