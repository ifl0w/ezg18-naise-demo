#include <Utils.hpp>
#include <spdlog/spdlog.h>
#include <stdarg.h>

std::string NAISE::Engine::Log::loggerPrefix(std::string file, int line) {
	std::string classname = file;

	std::size_t found = file.find_last_of("/\\");
	if(found < file.size()){
		classname = file.substr(found+1, file.size());
	} else if(file.find_last_of("\\") < file.size()){
		found = file.find_last_of("\\");
		classname = file.substr(found+1, file.size());
	}
	return classname + ":"+ std::to_string(line);
}



