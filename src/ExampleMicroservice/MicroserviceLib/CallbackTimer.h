#pragma once

#include <thread>
#include <chrono>

class CallbackTimer 
{
    bool clear = false;

public:
    template<typename Function>
    void setTimeout(int milli_delay, Function function)
    {
        this->clear = false;
        std::thread t([=]() 
        {
            if (this->clear) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(milli_delay));
            if (this->clear) return;
            function();
        });
        t.detach();
    }

    template<typename Function>
    void setInterval(int milli_interval, Function function)
    {
        this->clear = false;
        std::thread t([=]() 
        {
            while (true) 
            {
                if (this->clear) return;
                std::this_thread::sleep_for(std::chrono::milliseconds(milli_interval));
                if (this->clear) return;
                function();
            }
        });
        t.detach();
    }

    void stop() 
    {
        this->clear = true;
    }
};