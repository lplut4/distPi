#pragma once

#include <MicroserviceLib/MessageSubscriber.h>
#include <MicroserviceLib/CallbackTimer.h>
#include <MicroserviceLib/Publisher.h>
#include <MicroserviceLib/Logger.h>

#include "LogMessage.pb.h"

class ReactiveAgent
{
public:
	ReactiveAgent()
	{
		m_timeSubscriber.onMessage([this](auto message)
			{
				message->PrintDebugString();
			});

		m_timer.start(1000, [this]()
			{
				std::cout << "Tick!" << std::endl;
			});

		Logger::error(__FILELINE__, "Not really");
	};

	~ReactiveAgent() {};

private:

	CallbackTimer m_timer;
	MessageSubscriber<DataModel::TimeSpec> m_timeSubscriber;
};