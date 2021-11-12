#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <typeinfo>

#include "Logger.h"

// A threadsafe-queue.
template <class T>
class SafeQueue
{
public:
    SafeQueue( unsigned int capacity ) noexcept
        : q()
        , m()
        , c()
        , cap( capacity )
    {}

    ~SafeQueue()
    {}

    // Add an element to the queue.
    void enqueue(T t)
    {
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        
        if (q.size() > cap)
        {
            auto message = "Exceeding queue capacity for " + std::string( typeid(t).name() ) + ".  Dropping messages...";
            Logger::warning(__FILELINE__, message);
            q.pop();
        }
        
        c.notify_one();
    }

    // Get the "front"-element.
    // If the queue is empty, wait till a element is avaiable.
    T dequeue()
    {
        std::unique_lock<std::mutex> lock(m);
        while (q.empty())
        {
            // release lock as long as the wait and reaquire it afterwards.
            c.wait(lock);
        }
        T val = q.front();
        q.pop();
        return val;
    }

private:
    std::queue<T>           q;
    mutable std::mutex      m;
    std::condition_variable c;
    unsigned int            cap;
};