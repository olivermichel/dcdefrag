#ifndef CLOUDSIM_PHYSICAL_NODE_H
#define CLOUDSIM_PHYSICAL_NODE_H

#include <initializer_list>

#include "node.h"
#include "physical_resource.h"
#include "reservable.h"

namespace cloudsim {

	class PhysicalNode
		: public PhysicalResource, public Node, public interfaces::Reservable {

	public:
		PhysicalNode() = delete;

		PhysicalNode(unsigned capacity, unsigned long type = 0, unsigned long group = 0)
			: cloudsim::PhysicalResource(capacity, group),
			  cloudsim::Node(type),
			  cloudsim::interfaces::Reservable(capacity) { }

		PhysicalNode(std::initializer_list<unsigned> capacity, unsigned long type = 0, unsigned long group = 0)
			: cloudsim::PhysicalResource(capacity, group),
			  cloudsim::Node(type),
			  cloudsim::interfaces::Reservable(capacity) { }

		PhysicalNode(std::vector<unsigned> capacity, unsigned long type = 0, unsigned long group = 0)
			: cloudsim::PhysicalResource(capacity, group),
			  cloudsim::Node(type),
			  cloudsim::interfaces::Reservable(capacity) { }

		PhysicalNode(const PhysicalNode& copy_from);
	};
}

#endif
