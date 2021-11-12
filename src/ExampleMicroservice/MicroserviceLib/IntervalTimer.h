#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

// Fires off events at a defined interval
class IntervalTimer 
{   
public:

    IntervalTimer() noexcept
        : enabled( true )
        , mtx()
        , cvar()
    {}
    
    ~IntervalTimer()
    {
        enabled = false;
    }
    
    template<class _Rep, class _Period, typename Function>
    void onInterval(const std::chrono::duration<_Rep, _Period>& interval, Function function)
    {
        enabled = true;
        std::thread t([=]() 
        {           
            auto deadline = std::chrono::steady_clock::now() + interval;
            std::unique_lock<std::mutex> lock{mtx};
            while (enabled) 
            {
                if (cvar.wait_until(lock, deadline) == std::cv_status::timeout) 
                {
                    lock.unlock();
                    function();
                    deadline += interval;
                    lock.lock();
                }
            }
        });
        t.detach();
    }

    void stop() 
    {
        if (enabled) 
        {
            {
                std::lock_guard<std::mutex> _{mtx};
                enabled = false;
            }
            cvar.notify_one();
        }
    }
    
private:

    std::condition_variable cvar;
    std::atomic<bool>       enabled;
    mutable std::mutex      mtx;
};