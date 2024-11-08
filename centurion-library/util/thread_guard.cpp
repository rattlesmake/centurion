#include "thread_guard.h"

ThreadGuard::ThreadGuard(std::thread& _myThread)
    : myThread(_myThread)
{
}

ThreadGuard::~ThreadGuard(void)
{
    if (myThread.joinable() == true)
        myThread.join();
}