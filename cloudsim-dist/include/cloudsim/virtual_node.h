#ifndef CLOUDSIM_VIRTUAL_NODE_H
#define CLOUDSIM_VIRTUAL_NODE_H

#include "node.h"
#include "virtual_resource.h"
#include "touchable.h"
#include "virtual_network.h"

namespace cloudsim {

	class VirtualNetwork;

	class VirtualNode : public VirtualResource, public Node, public interfaces::Touchable {
	public:
		VirtualNode() = delete;

		VirtualNode(unsigned long id, unsigned demand, unsigned long type = 0, unsigned long group = 0,
					VirtualNetwork* network = nullptr);

		VirtualNode(unsigned long id, std::vector<unsigned> demand, unsigned long type = 0, unsigned long group = 0,
					VirtualNetwork* network = nullptr);

		VirtualNode(unsigned long id, std::initializer_list<unsigned> demand, unsigned long type = 0,
					unsigned long group = 0, VirtualNetwork* network = nullptr);

		VirtualNode(const VirtualNode& copy_from);

		void set_id(unsigned long id) { _id = id; }

		unsigned long id();

		VirtualNetwork* network();

	private:
		unsigned long _id;
		VirtualNetwork* _network;
	};
}

#endif
