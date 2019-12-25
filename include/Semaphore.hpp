#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
//#include <recursive_mutex>

namespace qyutil
{
using namespace std;
class Semaphore 
{
public:
    Semaphore(int count);
    virtual ~Semaphore();
    
    virtual void wait();
    virtual void post();
private:
public:
private:
    int m_count;
};

void semaphore_test();


}

#endif
