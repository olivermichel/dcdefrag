
#include <cloudsim/virtual_node.h>
#include <cloudsim/physical_node.h>
#include <cloudsim/virtual_edge.h>
#include <cloudsim/physical_edge.h>
#include <algorithm>
#include "cloudsim/greedy.h"

cloudsim::Greedy::Greedy(cloudsim::PhysicalNetwork& p, cloudsim::VirtualNetwork& v, bool debug,
						 std::ostream& os)
	: cloudsim::Algorithm(p, v, debug, os) { }

cloudsim::Mapping cloudsim::Greedy::operator()() throw (cloudsim::runtime_error)
{
	Mapping m;
	std::map<unsigned long, unsigned long> temp_node_map;
	std::map<unsigned long, std::vector<unsigned long>> temp_edge_map;

	for (auto i = _V.nodes.begin(); i != _V.nodes.end(); i++) {
		VirtualNode* vnode = dynamic_cast<VirtualNode*>(*i);

		for (auto j = _P.nodes.begin(); j != _P.nodes.end(); j++) {
			PhysicalNode* pnode = dynamic_cast<PhysicalNode*>(*j);

			if (vnode->demand() <= pnode->residual() && vnode->type() == pnode->type()) {
				vnode->touch();
				pnode->reserve(vnode->demand(0), 0);
				pnode->reserve(vnode->demand(1), 1);
				temp_node_map[i.id()] = j.id();
				break;
			}
		}
	}

	if (!_V.all_nodes([](VirtualNode* n) { return n->touched(); })) {
		_P.reset_node_reservations();
		_V.untouch_all_nodes();
		throw runtime_error("Greedy: node mapping failed.");
	}

	for (auto i = _V.edges.begin(); i != _V.edges.end(); i++) {

		VirtualEdge* vedge = dynamic_cast<VirtualEdge*>(*i);

		if (temp_node_map[i.from().id()] != temp_node_map[i.to().id()]) {

			try {
				auto path = _P.find_path(
					_P.nodes[temp_node_map[i.from().id()]], _P.nodes[temp_node_map[i.to().id()]],
					[&vedge] (Edge** e) {
						return dynamic_cast<PhysicalEdge*>(*e)->residual() >= vedge->demand();
					});

				temp_edge_map[i.id()] = {};

				for (auto j : path) {
					temp_edge_map[i.id()].push_back(j.id());
					dynamic_cast<PhysicalEdge*>(*j)->reserve(vedge->demand());
				}
				m.links.add(i.id(), temp_edge_map[i.id()]);
			} catch(plexum::Graph<cloudsim::Node*, cloudsim::Edge*>::exception& e) {
				_V.untouch_all_nodes();
				_P.reset_node_reservations();
				_P.reset_edge_reservations();
				throw runtime_error("Greedy: edge mapping failed.");
			}
		}
	}

	for (auto& k : temp_node_map) {
		if ((*(_V.nodes[k.first]))->type() == 0)
			m.switches.add(k.first, k.second);
		else
			m.servers.add(k.first, k.second);
	}

	return m;
}
