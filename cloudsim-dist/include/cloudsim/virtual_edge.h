#ifndef CLOUDSIM_VIRTUAL_EDGE_H
#define CLOUDSIM_VIRTUAL_EDGE_H

#include "edge.h"
#include "virtual_resource.h"
#include "virtual_network.h"

namespace cloudsim {

	class VirtualNetwork;

	class VirtualEdge : public VirtualResource, public Edge {
	public:
		VirtualEdge() = delete;
		VirtualEdge(unsigned long id, unsigned demand, unsigned long group = 0,
					VirtualNetwork* network = nullptr);
		VirtualEdge(const VirtualEdge& copy_from);

		void set_id(unsigned long id) { _id = id; }

		unsigned long id();

		VirtualNetwork* network();

	private:
		unsigned long _id;
		VirtualNetwork* _network;
	};
}

#endif
