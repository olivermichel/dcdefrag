
#include "cloudsim/edge.h"

cloudsim::Edge::Edge()
	: _type(0) { }

unsigned long cloudsim::Edge::type() const
{
	return _type;
}

cloudsim::Edge::~Edge()
{ }
