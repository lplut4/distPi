#pragma once

#include "google/protobuf/message.h"

namespace ExampleMicroservice
{
	static void queueUpPublish(const google::protobuf::Message& message);
}