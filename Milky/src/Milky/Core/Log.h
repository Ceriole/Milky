#pragma once

#include <glm/gtx/string_cast.hpp>

#include "Milky/Core/Base.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Milky {
	
	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; };
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; };

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::qua<T, Q>& quaternio)
{
	return os << glm::to_string(quaternio);
}

// CORE LOG MACROS ===========================================================
#define ML_CORE_TRACE(...)		::Milky::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ML_CORE_INFO(...)		::Milky::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ML_CORE_WARN(...)		::Milky::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ML_CORE_ERROR(...)		::Milky::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ML_CORE_CRITICAL(...)	::Milky::Log::GetCoreLogger()->critical(__VA_ARGS__)

// CLIENT LOG MACROS =========================================================
#define ML_TRACE(...)			::Milky::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ML_INFO(...)			::Milky::Log::GetClientLogger()->info(__VA_ARGS__)
#define ML_WARN(...)			::Milky::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ML_ERROR(...)			::Milky::Log::GetClientLogger()->error(__VA_ARGS__)
#define ML_CRITICAL(...)		::Milky::Log::GetClientLogger()->critical(__VA_ARGS__)
