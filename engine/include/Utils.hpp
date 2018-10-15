#pragma once

#include <string>
#include <spdlog/spdlog.h>

//#ifdef NAISE_DEBUG_ON
#define NAISE_DEBUG(logger, format, ...) NAISE::Engine::Log::debug(__FILE__,__LINE__, logger, format, ##__VA_ARGS__);
#define NAISE_WARN(logger, format, ...) NAISE::Engine::Log::warn(__FILE__,__LINE__, logger, format, ##__VA_ARGS__);
#define NAISE_ERROR(logger, format, ...) NAISE::Engine::Log::error(__FILE__,__LINE__, logger, format, ##__VA_ARGS__);
#define NAISE_DEBUG_PERSISTENCE(format, ...) NAISE_DEBUG("logger", format, ##__VA_ARGS__)
#define NAISE_WARN_PERSISTENCE(format, ...) NAISE_WARN("logger", format, ##__VA_ARGS__)
#define NAISE_ERROR_PERSISTENCE(format, ...) NAISE_ERROR("logger", format, ##__VA_ARGS__)
/*#else
#define NAISE_DEBUG(logger, format, ...) (void)0
#define NAISE_WARN(logger, format, ...) (void)0
#define NAISE_ERROR(logger, format, ...) (void)0
#endif*/

namespace NAISE {
namespace Engine {
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

}
}
}
