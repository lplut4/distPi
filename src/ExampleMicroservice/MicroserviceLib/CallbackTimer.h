#pragma once

#include <thread>
#include <chrono>

using interval_t = std::chrono::milliseconds;

class CallbackTimer 
{
private:

    std::atomic<bool> enabled;
	mutable std::mutex mtx;
	std::condition_variable cvar;
	
public:

	CallbackTimer()
	{
		enabled = true;
	}

    //template<typename Function>
    //void setTimeout(interval_t milli_delay, Function function)
    //{		
    //    std::thread t([=]() 
    //    {			
    //        auto deadline = std::chrono::steady_clock::now() + milli_delay;
    //        std::unique_lock<std::mutex> lock{mtx};
	//		if (cvar.wait_until(lock, deadline) == std::cv_status::timeout) 
	//		{
	//			lock.unlock();
	//			function();
	//			lock.lock();
	//		}
    //    });
    //    t.detach();
    //}

    template<typename Function>
    void setInterval(interval_t milli_interval, Function function)
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
};
