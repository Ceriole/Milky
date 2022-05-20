#pragma once

#include <string>

namespace Milky {

	class FileDialogs
	{
	public:
		// Both Open and Save return an empty string if cancelled
		static std::string Open(const char* filter);
		static std::string Save(const char* filter);
	};

}
