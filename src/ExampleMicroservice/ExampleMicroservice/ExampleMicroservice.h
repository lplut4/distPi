#pragma once

#include "google/protobuf/message.h"
#include <string>
#include "MessageSubscriber.h"

namespace ExampleMicroservice
{
	// All subscribers must be registered before starting the service
	static void registerSubscriber(std::string channel, IMessageSubscriber* sub);

	// Connect to the message broker, and begin processing pub-sub messages
	static int startService(int argc, char* argv[]);

	// Publishes a message to all subscribers of this message type
	static void queueUpPublish(const google::protobuf::Message& message);
}