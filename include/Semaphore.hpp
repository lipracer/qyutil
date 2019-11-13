#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <thread>
#include <atomic>

namespace qyutil
{
using namespace std;
class Semaphore 
{
public:
    Semaphore(int count);
    virtual ~Semaphore();
    
    virtual void wait();
private:
public:
private:
    atomic<int> m_count;
};

}

#endif
