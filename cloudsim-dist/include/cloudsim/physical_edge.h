#ifndef CLOUDSIM_PHYSICAL_EDGE_H
#define CLOUDSIM_PHYSICAL_EDGE_H

#include "edge.h"
#include "physical_resource.h"
#include "reservable.h"

namespace cloudsim {

	class PhysicalEdge : public PhysicalResource, public Edge, public interfaces::Reservable {

	public:
		PhysicalEdge() = delete;
		PhysicalEdge(double capacity, unsigned long group = 0)
			: cloudsim::PhysicalResource(capacity, group),
			  cloudsim::Edge(),
			  interfaces::Reservable(capacity) { }
		PhysicalEdge(const PhysicalEdge& copy_from);
	};
}

#endif
