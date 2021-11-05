#pragma once

#include <MicroserviceLib/MessageSubscriber.h>
#include <MicroserviceLib/CallbackTimer.h>
#include <MicroserviceLib/Publisher.h>
#include <MicroserviceLib/Logger.h>

#include "LogMessage.pb.h"

class ReactiveAgent
{
public:
	ReactiveAgent() noexcept
	{
		m_timeSubscriber.onMessage([this](auto message)
		{
			message->PrintDebugString();
		});

		m_timer.setInterval(1000, [this]()
		{
			std::cout << "Tick!" << std::endl;
			timerCount++;

			if (timerCount > 5)
			{
				m_timer.stop();
			}
		});

		Logger::info(__FILELINE__, "Started ReactiveAgent");
	};

	~ReactiveAgent() {};

private:

	int timerCount = 0;
	CallbackTimer m_timer;
	MessageSubscriber<DataModel::TimeSpec> m_timeSubscriber;
};