#pragma once

#include "Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Neko {
	class NEKO_API Log {
	public:
		
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_clientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;
	};
}

// core log macros
#define NEKO_CORE_ERROR(...)	::Neko::Log::GetCoreLogger()->error(__VA_ARGS__)
#define NEKO_CORE_WARN(...)		::Neko::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define NEKO_CORE_INFO(...)		::Neko::Log::GetCoreLogger()->info(__VA_ARGS__)
#define NEKO_CORE_TRACE(...)	::Neko::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define NEKO_CORE_FATAL(...)	::Neko::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// client log macros
#define NEKO_ERROR(...)	::Neko::Log::GetClientLogger()->error(__VA_ARGS__)
#define NEKO_WARN(...)	::Neko::Log::GetClientLogger()->warn(__VA_ARGS__)
#define NEKO_INFO(...)	::Neko::Log::GetClientLogger()->info(__VA_ARGS__)
#define NEKO_TRACE(...)	::Neko::Log::GetClientLogger()->trace(__VA_ARGS__)
#define NEKO_FATAL(...)	::Neko::Log::GetClientLogger()->fatal(__VA_ARGS__)

