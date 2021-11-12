#pragma once

#include <chrono>
#include <MicroserviceLib/IntervalTimer.h>
#include <MicroserviceLib/Logger.h>
#include <MicroserviceLib/MessageSubscriber.h>
#include <MicroserviceLib/Publisher.h>

#include "Ping.pb.h"
#include "Pong.pb.h"

using namespace std::chrono_literals;

class ReactiveAgent
{
private:

    int           m_timerCount = 0;
    IntervalTimer m_tickTimer;

    MessageSubscriber<DataModel::Test::Ping> m_pingSubscriber;
    MessageSubscriber<DataModel::Test::Pong> m_pongSubscriber;

public:
    ReactiveAgent() noexcept
        : m_pingSubscriber( 1 )
        , m_pongSubscriber( 1 )
    {
        m_pingSubscriber.onMessage([this](auto message)
            {
                const uint64_t count = message->count() + 1;
                std::cout << "Pong " << count << std::endl;
                std::this_thread::sleep_for(1000ms);
                DataModel::Test::Pong reply;
                reply.set_count(count);
                Publisher::addToQueue(reply);
            });

        m_pongSubscriber.onMessage([this](auto message)
            {
                const uint64_t count = message->count() + 1;
                std::cout << "Ping " << count << std::endl;
                std::this_thread::sleep_for(1000ms);
                DataModel::Test::Ping reply;
                reply.set_count(count);
                Publisher::addToQueue(reply);
            });

        m_tickTimer.onInterval(1000ms, [this]()
            {
                std::cout << "Tick!" << std::endl;
                m_timerCount++;
                if (m_timerCount > 5)
                {
                    m_tickTimer.stop();
                }
            });

        Logger::info(__FILELINE__, "Started ReactiveAgent");

        std::cout << "Ping 0" << std::endl;
        DataModel::Test::Ping serve;
        serve.set_count(0);
        Publisher::addToQueue(serve);
    };

    ~ReactiveAgent() {};
};
