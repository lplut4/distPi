#pragma once

#include "google/protobuf/message.h"

namespace Publisher
{
	// Publishes a message to all subscribers of this message type
	void addToQueue(const google::protobuf::Message& message);
}
