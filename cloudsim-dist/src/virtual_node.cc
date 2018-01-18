
#include "cloudsim/virtual_node.h"

cloudsim::VirtualNode::VirtualNode(unsigned long id, unsigned demand, unsigned long type,
								   unsigned long group, cloudsim::VirtualNetwork* network)
	: cloudsim::VirtualResource(demand, group),
	  cloudsim::Node(type),
	  cloudsim::interfaces::Touchable(),
	  _id(id),
	  _network(network)
{ }

cloudsim::VirtualNode::VirtualNode(unsigned long id, std::vector<unsigned> demand,
								   unsigned long type, unsigned long group,
								   cloudsim::VirtualNetwork* network)
	: cloudsim::VirtualResource(demand, group),
	  cloudsim::Node(type),
	  cloudsim::interfaces::Touchable(),
	  _id(id),
	  _network(network)
{ }

cloudsim::VirtualNode::VirtualNode(unsigned long id, std::initializer_list<unsigned> demand,
								   unsigned long type, unsigned long group,
								   cloudsim::VirtualNetwork* network)
	: cloudsim::VirtualResource(demand, group),
	  cloudsim::Node(type),
	  cloudsim::interfaces::Touchable(),
	  _id(id),
	  _network(network)
{ }

cloudsim::VirtualNode::VirtualNode(const VirtualNode& copy_from)
	: cloudsim::VirtualResource(copy_from),
	  cloudsim::Node(copy_from),
	  cloudsim::interfaces::Touchable(copy_from),
	  _id(copy_from._id),
	  _network(copy_from._network)
{ }

unsigned long cloudsim::VirtualNode::id()
{
	return _id;
}

cloudsim::VirtualNetwork* cloudsim::VirtualNode::network()
{
	return _network;
}
