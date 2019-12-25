#include "Semaphore.hpp"

#include <semaphore.h>

#include <iostream>
#include <memory>

namespace qyutil
{

Semaphore::Semaphore(int count) : m_count(count)
{
    
}

Semaphore::~Semaphore()
{
    
}

void Semaphore::wait()
{
    sem_wait(&m_count);
}

void Semaphore::post()
{
    sem_post(&m_count);
}

void semaphore_test()
{
    shared_ptr<Semaphore> semaphore = make_shared<Semaphore>(3);
    
    auto task =[](shared_ptr<Semaphore> semaphore){
        while (true)
        {
            semaphore->wait();
            cout << "start do task:"<< this_thread::get_id() << endl;
            this_thread::sleep_for(chrono::seconds(10));
            cout << "end do task" << endl;
            semaphore->post();
        }
        
    };
    
    thread ths[10];
    for (auto & it : ths)
    {
        it = thread(task, semaphore);
        it.detach();
    }
}

}
