#pragma once

#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

//#ifdef NAISE_DEBUG_ON
#define NAISE_DEBUG(logger, format, ...) NAISE::Engine::Log::debug(__FILE__,__LINE__, logger, format, ##__VA_ARGS__);
#define NAISE_WARN(logger, format, ...) NAISE::Engine::Log::warn(__FILE__,__LINE__, logger, format, ##__VA_ARGS__);
#define NAISE_ERROR(logger, format, ...) NAISE::Engine::Log::error(__FILE__,__LINE__, logger, format, ##__VA_ARGS__);
#define NAISE_INFO(logger, format, ...) NAISE::Engine::Log::info(__FILE__,__LINE__, logger, format, ##__VA_ARGS__);

#define NAISE_WARN_LOG(format, ...) NAISE_WARN("logger", format, ##__VA_ARGS__)
#define NAISE_ERROR_LOG(format, ...) NAISE_ERROR("logger", format, ##__VA_ARGS__)
#define NAISE_DEBUG_CONSOL(format, ...) NAISE_DEBUG("console", format, ##__VA_ARGS__)
#define NAISE_WARN_CONSOL(format, ...) NAISE_WARN("console", format, ##__VA_ARGS__)
#define NAISE_ERROR_CONSOL(format, ...) NAISE_ERROR("console", format, ##__VA_ARGS__)
#define NAISE_INFO_CONSOL(format, ...) NAISE_INFO("console", format, ##__VA_ARGS__)
/*#else
#define NAISE_DEBUG(logger, format, ...) (void)0
#define NAISE_WARN(logger, format, ...) (void)0
#define NAISE_ERROR(logger, format, ...) (void)0
#endif*/

namespace NAISE {
namespace Engine {

class Logger {
public:

	Logger(){
		/*
 		* Use the logger "console" to log messages that are not suitable for the log file.
 		*/
		auto console = spdlog::stdout_color_mt("console");
		console->set_level(spdlog::level::debug);

		auto logger = spdlog::stdout_color_mt("logger");
		logger->set_level(spdlog::level::warn);
	};
};

namespace Log {
std::string loggerPrefix(std::string file, int line);

template<typename  ... Args>
void debug(std::string file, int line, std::string logger, std::string format, Args ...args) {
	std::string prefix = loggerPrefix(file, line);
	spdlog::get(logger)->debug(std::string("{} >> ").append(format).c_str(), prefix, args...);
};

template<typename  ... Args>
void warn(std::string file, int line, std::string logger, std::string format, Args ...args) {
	std::string prefix = loggerPrefix(file, line);
	spdlog::get(logger)->warn(std::string("{} >> ").append(format).c_str(), prefix, args...);
};

template<typename  ... Args>
void error(std::string file, int line, std::string logger, std::string format, Args ...args) {
	std::string prefix = loggerPrefix(file, line);
	spdlog::get(logger)->error(std::string("{} >> ").append(format).c_str(), prefix, args...);
};

template<typename  ... Args>
void info(std::string file, int line, std::string logger, std::string format, Args ...args) {
	std::string prefix = loggerPrefix(file, line);
	spdlog::get(logger)->info(std::string("{} >> ").append(format).c_str(), prefix, args...);
};

}
}
}
