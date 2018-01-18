
#include "cloudsim/virtual_resource.h"

cloudsim::VirtualResource::VirtualResource(unsigned demand, unsigned long group)
	: _demand({demand}), _group(group)
{ }


cloudsim::VirtualResource::VirtualResource(std::initializer_list<unsigned> demand, unsigned long group)
	: _demand(demand.begin(), demand.end()), _group(group)
{ }

cloudsim::VirtualResource::VirtualResource(std::vector<unsigned> demand, unsigned long group)
	: _demand(demand), _group(group)
{ }

unsigned cloudsim::VirtualResource::demand(unsigned long i) const
	throw (cloudsim::logic_error)
{
	if (i >= _demand.size())
		throw cloudsim::logic_error("cloudsim::VirtualResourceResource::demand(): index out of range");

	return _demand[i];
}

unsigned long cloudsim::VirtualResource::group() const
{
	return _group;
}

cloudsim::VirtualResource::~VirtualResource()
{ }