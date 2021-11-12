#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

// Fires off a single event after a duration has elapsed
class TerminalTimer 
{   
public:

    TerminalTimer() noexcept
        : mtx()
        , cvar()
    {}
    
    ~TerminalTimer()
    {}
    
    template<class _Rep, class _Period, typename Function>
    void onInterval(const std::chrono::duration<_Rep, _Period>& delay, Function function)
    {
        std::thread t([=]() 
        {                   
            auto deadline = std::chrono::steady_clock::now() + delay;
            std::unique_lock<std::mutex> lock{mtx};
            if (cvar.wait_until(lock, deadline) == std::cv_status::timeout) 
            {
                lock.unlock();
                function();
            }
        });
        t.detach();
    }

    void stop() noexcept
    {
        cvar.notify_one();
    }
    
private:

    std::condition_variable cvar;
    mutable std::mutex      mtx;  
    std::thread             timerThread;
};