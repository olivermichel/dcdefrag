
#include "cloudsim/physical_node.h"

cloudsim::PhysicalNode::PhysicalNode(const cloudsim::PhysicalNode& copy_from)
	: cloudsim::PhysicalResource(copy_from),
	  cloudsim::Node(),
	  cloudsim::interfaces::Reservable(copy_from)
{ }
