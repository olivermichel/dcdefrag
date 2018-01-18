
#ifndef VDE_UTIL_H
#define VDE_UTIL_H

#include <vector>
#include <stdexcept>
#include <string>

#include <chrono>

namespace util {
	std::vector<std::string> filenames_in_directory(const std::string& directory_name)
		throw(std::runtime_error);

	template<typename F>
	double duration(F f)
	{
		std::chrono::time_point<std::chrono::system_clock> start, stop;
		std::chrono::duration<double> duration;
		start = std::chrono::system_clock::now();
		f();
		stop = std::chrono::system_clock::now();
		duration = stop - start;
		return duration.count();
	}
}

#endif
