#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

namespace Neko {

	struct ProfileResult {
		std::string name;
		long long st, ed;
		uint32_t threadId;
	};

	struct InstrumentationSession {
		std::string name;
	};

	class Instrumentor {
	private:
		InstrumentationSession* m_curSession = nullptr;
		std::ofstream m_outputStream;
		int m_profileCount = 0;
	public:
		Instrumentor() {}

		void BeginSession(const std::string& name, const std::string& filePath = "result.json") {
			m_outputStream.open(filePath);
			WriteHeader();
			m_curSession = new InstrumentationSession{ name };
		}

		void EndSession() {
			WriteFooter();
			m_outputStream.close();

			delete m_curSession;
			m_curSession = nullptr;
			m_profileCount = 0;
		}

		void WriteProfile(const ProfileResult& result) {
			if (m_profileCount > 0) {
				m_outputStream << ",";
			}

			m_profileCount++;
			std::string name = result.name;
			std::replace(name.begin(), name.end(), '"', '\'');
			m_outputStream << "{";
			m_outputStream << "\"cat\":\"function\",";
			m_outputStream << "\"dur\":" << (result.ed - result.st) << ',';
			m_outputStream << "\"name\":\"" << name << "\",";
			m_outputStream << "\"ph\":\"X\",";
			m_outputStream << "\"pid\":0,";
			m_outputStream << "\"tid\":" << result.threadId << ",";
			m_outputStream << "\"ts\":" << result.st;
			m_outputStream << "}";

			m_outputStream.flush();
		}

		void WriteHeader() {
			m_outputStream << "{\"OtherData\": {}, \"TraceEvents\":[";
			m_outputStream.flush();
		}

		void WriteFooter() {
			m_outputStream << "]}";
			m_outputStream.flush();
		}

		static Instrumentor& GetCurrent() {
			static Instrumentor instance;
			return instance;
		}
	};

	class InstrumentationTimer {
	public:
		InstrumentationTimer(const char* name) : m_name(name) {
			m_startTimePoint = std::chrono::high_resolution_clock::now();
		}

		~InstrumentationTimer() {
			if (!m_stopped)
				Stop();
		}

		void Stop() {
			auto endTimePoint = std::chrono::high_resolution_clock::now();

			long long st = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch().count();
			long long ed = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

			uint32_t threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
			Instrumentor::GetCurrent().WriteProfile({ m_name, st, ed, threadId });

			m_stopped = true;
			// m_func(ProfileResult({ m_name, st, ed, threadId }));
		}

	private:
		const char* m_name;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimePoint;
		// Fn m_func;
		bool m_stopped = false;
	};
}


#define NEKO_PROFILE 1
#ifdef NEKO_PROFILE
	#define NEKO_PROFILE_BEGIN_SESSION(name, filepath) ::Neko::Instrumentor::GetCurrent().BeginSession(name, filepath)
	#define NEKO_PROFILE_END_SESSION() ::Neko::Instrumentor::GetCurrent().EndSession()
	#define NEKO_PROFILE_SCOPE(name) ::Neko::InstrumentationTimer time##__LINE__(name)
	#define NEKO_PROFILE_FUNCTION() NEKO_PROFILE_SCOPE(__FUNCSIG__)
#else
	#define NEKO_PROFILE_BEGIN_SESSION(name, filepath)
	#define NEKO_PROFILE_END_SESSION()
	#define NEKO_PROFILE_SCOPE(name)
	#define NEKO_PROFILE_FUNCTION()
#endif // NEKO_PROFILE
