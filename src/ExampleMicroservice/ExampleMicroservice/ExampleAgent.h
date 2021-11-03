#pragma once

#include "MessageSubscriber.h"
#include "CallbackTimer.h"
#include "Publisher.h"
#include "LogMessage.pb.h"

class ExampleAgent
{
public:
	ExampleAgent() 
	{
		m_subscriber.onMessage([this](std::shared_ptr<DataModel::LogMessage> message)
			{
				processMessage(message);
			});
	
		//m_timer.start(1000, [this]() 
		//	{ 
		//		timerEvent(); 
		//	});

		//m_timerCount = 0;
	};

	~ExampleAgent() {};

private:
	
	void processMessage(std::shared_ptr<DataModel::LogMessage> message)
	{
		//std::cout << "AGENT: ";
		//message->PrintDebugString();
		
		DataModel::LogMessage nemeee;
		
		Publisher::addToQueue(nemeee);
	}
	
	//void timerEvent()
	//{
	//	m_timerCount++;
	//	std::cout << "Tick!" << std::endl;
	//	if (m_timerCount > 5)
	//	{
	//		m_timer.stop();
	//	}
	//}

	//int m_timerCount;
	//CallbackTimer m_timer;
	MessageSubscriber<DataModel::LogMessage> m_subscriber;
};