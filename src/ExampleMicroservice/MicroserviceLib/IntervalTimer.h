#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

using interval_t = std::chrono::milliseconds;

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
	
    template<typename Function>
    void onInterval(interval_t milli_interval, Function function)
    {
		enabled = true;
        std::thread t([=]() 
        {			
			auto deadline = std::chrono::steady_clock::now() + milli_interval;
            std::unique_lock<std::mutex> lock{mtx};
            while (enabled) 
            {
                if (cvar.wait_until(lock, deadline) == std::cv_status::timeout) 
				{
                    lock.unlock();
                    function();
                    deadline += milli_interval;
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
    std::atomic<bool> 		enabled;
	mutable std::mutex      mtx;

};