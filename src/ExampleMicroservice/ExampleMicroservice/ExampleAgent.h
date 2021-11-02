#pragma once

#include "LogMessage.pb.h"
#include "MessageSubscriber.h"

class ExampleAgent
{
public:
	ExampleAgent() 
	{
		subscriber.onMessage([this](std::shared_ptr<DataModel::LogMessage> message)
			{
				processMessage(message);
			});
	
	};

	~ExampleAgent() {};

private:
	
	void processMessage(std::shared_ptr<DataModel::LogMessage> message)
	{
		std::cout << "AGENT: ";
		message->PrintDebugString();
	}

	MessageSubscriber<DataModel::LogMessage> subscriber;
};