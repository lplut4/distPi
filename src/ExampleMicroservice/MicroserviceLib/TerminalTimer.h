#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

using interval_t = std::chrono::milliseconds;

class TerminalTimer 
{	
public:

	TerminalTimer() noexcept
		: mtx()
		, cvar()
	{}
	
	~TerminalTimer()
	{}
	
    template<typename Function>
    void onTimeout(interval_t milli_delay, Function function)
    {
		std::thread t([=]() 
        {					
			auto deadline = std::chrono::steady_clock::now() + milli_delay;
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