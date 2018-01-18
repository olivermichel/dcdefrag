
#include "cloudsim/runtime_error.h"

cloudsim::runtime_error::runtime_error(const char* what)
	: std::exception(), _what(what) { }
cloudsim::runtime_error::runtime_error(const std::string& what)
	: std::exception(), _what(what) { }

const char* cloudsim::runtime_error::what() const throw()
{
	return _what.c_str();
}

cloudsim::runtime_error::~runtime_error() throw() { }
