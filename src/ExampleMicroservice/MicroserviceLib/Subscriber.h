#pragma once

#include "IMessageSubscriber.h"

namespace Subscriber
{
	// All subscribers must be registered before starting the service
	void registerSubscriber(const std::string& channel, IMessageSubscriber* sub);
}