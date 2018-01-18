
#ifndef CLOUDSIM_RUNTIME_ERROR_H
#define CLOUDSIM_RUNTIME_ERROR_H

#include <stdexcept>
#include <string>

namespace cloudsim {

	class runtime_error : public std::exception {
	public:
		runtime_error() = delete;
		explicit runtime_error(const char* what);
		explicit runtime_error(const std::string& what);
		virtual const char* what() const throw();
		virtual ~runtime_error() throw();

	private:
		std::string _what;
	};
}

#endif
