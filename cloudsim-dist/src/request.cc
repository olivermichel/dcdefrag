
#include "cloudsim/request.h"

cloudsim::Request::Request(unsigned long id)
	: _id(id) { }

unsigned long cloudsim::Request::id() const
{
	return _id;
}

cloudsim::Request::~Request() { }