#pragma once

#include <string>
#include "ExampleMicroservice.h"
#include "LogMessage.pb.h"

namespace Logger
{
	void logError(const std::string& message) {};

	void logWarning(const std::string& message) {};

	void logInfo(const std::string& msg)
	{
		DataModel::LogMessage logMessage;

		struct timespec ts;

		auto retVal = timespec_get(&ts, TIME_UTC);

		auto spec = new DataModel::TimeSpec();
		spec->set_tv_sec(ts.tv_sec);
		spec->set_tv_nsec(ts.tv_nsec);

		auto message = new std::string(msg);

		logMessage.set_allocated_timeoflog(spec);
		logMessage.set_category(DataModel::LogMessage_Category_INFO);
		logMessage.set_allocated_message(message);

		//ExampleMicroservice::queueUpPublish(logMessage);

		std::cout << "INFO: " << msg << std::endl;
	}
}