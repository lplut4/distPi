#pragma once

#include <string> 

#define __FILELINE__	__FILE__, __LINE__

namespace Logger
{
	void error   (const char* file, const int line, const char* message);
	void warning (const char* file, const int line, const char* message);
	void info    (const char* file, const int line, const char* message);

	void error	(const char* file, const int line, const std::string& message);
	void warning(const char* file, const int line, const std::string& message);
	void info	(const char* file, const int line, const std::string& message);
}