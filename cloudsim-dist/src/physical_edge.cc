
#include "cloudsim/physical_edge.h"

cloudsim::PhysicalEdge::PhysicalEdge(const cloudsim::PhysicalEdge& copy_from)
	: cloudsim::PhysicalResource(copy_from),
	  cloudsim::Edge(),
	  cloudsim::interfaces::Reservable(copy_from)
{ }
