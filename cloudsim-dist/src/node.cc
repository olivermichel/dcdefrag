
#include "cloudsim/node.h"

cloudsim::Node::Node(unsigned long type)
	: _type(type) { }

unsigned long cloudsim::Node::type() const
{
	return _type;
}

cloudsim::Node::~Node()
{ }
