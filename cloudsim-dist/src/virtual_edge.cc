
#include <cloudsim/virtual_network.h>
#include "cloudsim/virtual_edge.h"

cloudsim::VirtualEdge::VirtualEdge(unsigned long id, unsigned demand, unsigned long group,
								   cloudsim::VirtualNetwork* network)
	: cloudsim::VirtualResource(demand, group),
	  cloudsim::Edge(),
	  _id(id),
	  _network(network)
{ }

cloudsim::VirtualEdge::VirtualEdge(const VirtualEdge& copy_from)
	: cloudsim::VirtualResource(copy_from),
	  cloudsim::Edge(),
	  _id(copy_from._id),
	  _network(copy_from._network)

{ }

unsigned long cloudsim::VirtualEdge::id()
{
	return _id;
}

cloudsim::VirtualNetwork* cloudsim::VirtualEdge::network()
{
	return _network;
}

