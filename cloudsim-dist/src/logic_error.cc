
#include "cloudsim/logic_error.h"

cloudsim::logic_error::logic_error(const char* what)
	: std::exception(), _what(what) { }
cloudsim::logic_error::logic_error(const std::string& what)
	: std::exception(), _what(what) { }

const char* cloudsim::logic_error::what() const throw()
{
	return _what.c_str();
}

cloudsim::logic_error::~logic_error() throw() { }
