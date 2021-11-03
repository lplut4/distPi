#pragma once

#include <string>

namespace Logger
{
	void error(const std::string& message);
	void warning(const std::string& message);
	void info(const std::string& message);
}