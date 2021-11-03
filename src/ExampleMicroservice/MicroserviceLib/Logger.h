#pragma once

#include <string>

namespace Logger
{
	void logError(const std::string& message);
	void logWarning(const std::string& message);
	void logInfo(const std::string& message);
}