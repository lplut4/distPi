#pragma once

#include "impl/IMessageSubscriber.h"

namespace Subscriber
{
	// All subscribers must be registered before starting the service
	bool registerSubscriber(const std::string& channel, IMessageSubscriber* sub);
	
	// Prevent dead letters by unsubscribing when messages will no longer be consumed
	void unregisterSubscriber(const std::string& channel, IMessageSubscriber* sub);
}