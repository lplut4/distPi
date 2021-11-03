#pragma once

class CallbackTimer
{
public:
    CallbackTimer() : m_execute(false) {}

    ~CallbackTimer() 
    {
        if ( m_execute.load( std::memory_order_acquire ) ) 
        {
            stop();
        };
    }

    void stop()
    {
        m_execute.store(false, std::memory_order_release);
        if ( m_thread.joinable() )
        {
            m_thread.join();
        }
    }

    void start(int interval_milli, std::function<void(void)> func)
    {
        if ( m_execute.load( std::memory_order_acquire ) ) 
        {
            stop();
        };
        m_execute.store( true, std::memory_order_release );
        m_thread = std::thread( [this, interval_milli, func] ()
            {
                while ( m_execute.load( std::memory_order_acquire ) ) 
                {
                    std::this_thread::sleep_for( std::chrono::milliseconds( interval_milli ) );
                    func();
                }
            });
    }

    bool is_running() const noexcept 
    {
        return ( m_execute.load( std::memory_order_acquire ) && m_thread.joinable() );
    }

private:
    std::atomic<bool> m_execute;
    std::thread       m_thread;
};