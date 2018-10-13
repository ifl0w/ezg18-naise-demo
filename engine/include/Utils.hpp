#pragma once

#include <string>
#include <spdlog/spdlog.h>

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
