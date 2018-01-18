
#ifndef CLOUDSIM_LOGIC_ERROR_H
#define CLOUDSIM_LOGIC_ERROR_H

#include <stdexcept>
#include <string>

namespace cloudsim {

	class logic_error : public std::exception {
	public:
		logic_error() = delete;
		explicit logic_error(const char* what);
		explicit logic_error(const std::string& what);
		virtual const char* what() const throw();
		virtual ~logic_error() throw();

	private:
		std::string _what;
	};
}

#endif
