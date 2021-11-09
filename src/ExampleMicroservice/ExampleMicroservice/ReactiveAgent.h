#pragma once

#include <chrono>
#include <MicroserviceLib/MessageSubscriber.h>
#include <MicroserviceLib/CallbackTimer.h>
#include <MicroserviceLib/Publisher.h>
#include <MicroserviceLib/Logger.h>

#include "Ping.pb.h"
#include "Pong.pb.h"
#include "LogMessage.pb.h"

using namespace std::chrono_literals;

class ReactiveAgent
{
public:
	ReactiveAgent() noexcept
	{
		m_pingSubscriber.onMessage([](auto message)
			{
				uint64_t count = message->count() + 1;

				std::cout << "Pong " << count << std::endl;

				std::this_thread::sleep_for(1000ms);
				
				DataModel::Test::Pong reply;
				reply.set_count(count);
				Publisher::addToQueue(reply);
			});

		m_pongSubscriber.onMessage([](auto message)
			{
				uint64_t count = message->count() + 1;

				std::cout << "Ping " << count << std::endl;

				std::this_thread::sleep_for(1000ms);

				DataModel::Test::Ping reply;
				reply.set_count(count);
				Publisher::addToQueue(reply);
			});

		m_timer.setInterval(1000ms, [this]()
			{
				std::cout << "Tick!" << std::endl;
				m_timerCount++;
				if (m_timerCount > 5)
				{
					m_timer.stop();
				}
			});

		Logger::info(__FILELINE__, "Started ReactiveAgent");

		DataModel::Test::Ping serve;
		serve.set_count(0);
		Publisher::addToQueue(serve);

		std::cout << "Ping 0" << std::endl;
	};

	~ReactiveAgent() {};

private:

	int m_timerCount = 0;
	CallbackTimer m_timer;

	MessageSubscriber<DataModel::Test::Ping> m_pingSubscriber;
	MessageSubscriber<DataModel::Test::Pong> m_pongSubscriber;
};
