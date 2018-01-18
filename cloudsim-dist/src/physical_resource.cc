
#include <initializer_list>

#include "cloudsim/physical_resource.h"

cloudsim::PhysicalResource::PhysicalResource(unsigned capacity, unsigned long group)
	: _capacity({capacity}), _group(group)
{ }

cloudsim::PhysicalResource::PhysicalResource(std::initializer_list<unsigned> capacity, unsigned long group)
	: _capacity(capacity.begin(), capacity.end()), _group(group)
{ }

cloudsim::PhysicalResource::PhysicalResource(std::vector<unsigned> capacity, unsigned long group)
	: _capacity(capacity), _group(group)
{ }

unsigned cloudsim::PhysicalResource::capacity(unsigned long i) const
	throw (cloudsim::logic_error)
{
	if (i >= _capacity.size())
		throw cloudsim::logic_error("cloudsim::PhysicalResource::capacity(): index out of range");

	return _capacity[i];
}

unsigned long cloudsim::PhysicalResource::group() const
{
	return _group;
}

cloudsim::PhysicalResource::~PhysicalResource()
{ }