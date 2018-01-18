
#include "tavdce.h"

#include <iostream>

#include "../../util/bipartite_matching.h"

//#include <cloudsim/physical_node.h>
//#include <cloudsim/physical_edge.h>

namespace cs = cloudsim;
using namespace cs;


algo::TAVDCE::TAVDCE(cs::PhysicalNetwork& p, cs::VirtualNetwork& v,
								 std::vector<std::vector<int>>& d, bool debug, std::ostream& os)
	: cs::Algorithm(p, v, debug, os), _Sp(), _Xp(), _Ep(), _Sv(), _Xv(), _Ev(), _d(d)
{
	// build vector of pointers to physical servers and switches
	for (auto node_it = _P.vertices.begin(); node_it != _P.vertices.end(); node_it++) {

		if (dynamic_cast<cs::PhysicalNode*>(*node_it)->type() == TYPE_SERVER)
			_Sp.push_back(node_it.id());
		else if (dynamic_cast<cs::PhysicalNode*>(*node_it)->type() == TYPE_SWITCH)
			_Xp.push_back(node_it.id());
	}

	// build vector of pointers to virtual servers and switches
	for (auto node_it = _V.vertices.begin(); node_it != _V.vertices.end(); node_it++) {

		if (dynamic_cast<cs::VirtualNode*>(*node_it)->type() == TYPE_SERVER)
			_Sv.push_back(node_it.id());
		else if (dynamic_cast<cs::VirtualNode*>(*node_it)->type() == TYPE_SWITCH)
			_Xv.push_back(node_it.id());
	}

	// build vector of pointers to physical edges
	for (auto edge_it = _P.edges.begin(); edge_it != _P.edges.end(); edge_it++)
		_Ep.push_back(edge_it.id());

	// build vector of pointers to virtual edges
	for (auto edge_it = _V.edges.begin(); edge_it != _V.edges.end(); edge_it++)
		_Ev.push_back(edge_it.id());
}

cloudsim::Mapping algo::TAVDCE::operator()() throw (cs::runtime_error)
{
	cs::Mapping m;

	std::map<unsigned long, unsigned long> s_map;
	std::map<unsigned long, unsigned long> x_map;
	std::map<unsigned long, std::vector<unsigned long>> e_map;

	for (unsigned i = 0; i < _Sp.size(); i++) { // [1]

		std::vector<unsigned long> Sp_s(_Sp.begin(), _Sp.begin() + i + 1);

		try {
			if (_debug) _os << "vm mapping:" << std::endl;
			s_map = _vm_map(Sp_s);
		} catch (cs::runtime_error& e) {
			if (_debug) std::cout << "  vm mapping failed" << std::endl;
			continue;
		}

		try {
			if (_debug) _os << "sw mapping:" << std::endl;
			x_map = _sw_map(s_map);
		} catch (cs::runtime_error& e) {
			if (_debug) std::cout << "  sw mapping failed" << std::endl;
			continue;
		}

		try {
			if (_debug) _os << "link mapping:" << std::endl;
			std::map<unsigned long, unsigned long> sx_map;

			// combine server and switch mappings to new sx_map map
			sx_map.insert(s_map.begin(), s_map.end());
			sx_map.insert(x_map.begin(), x_map.end());
			e_map = _link_map(sx_map);
			break;
		} catch (std::runtime_error& e) {
			if (_debug) _os << "  link mapping failed";
			continue;
		}
	}

	if (s_map.empty() || x_map.empty() || e_map.empty())
		throw cs::runtime_error("embedding failed");

	for (auto s : s_map)
		m.servers.add(s.first, s.second);

	for (auto x : x_map)
		m.switches.add(x.first, x.second);

	for (auto e : e_map)
		m.links.add(e.first, e.second);

	_V.untouch_all_nodes();

	return m;
}

cloudsim::Mapping algo::TAVDCE::operator()(cloudsim::Mapping& old_map)
	throw(cloudsim::runtime_error)
{
	cs::Mapping m;

	std::map<unsigned long, unsigned long> s_map;
	std::map<unsigned long, unsigned long> x_map;
	std::map<unsigned long, std::vector<unsigned long>> e_map;

	for (unsigned i = 0; i < _Sp.size(); i++) { // [1]

		std::vector<unsigned long> Sp_s(_Sp.begin(), _Sp.begin() + i + 1);

		try {
			if (_debug) _os << "vm mapping:" << std::endl;
			s_map = _vm_map(Sp_s, old_map);
		} catch (cs::runtime_error& e) {
			if (_debug) std::cout << "  vm mapping failed" << std::endl;
			continue;
		}

		try {
			if (_debug) _os << "sw mapping:" << std::endl;
			x_map = _sw_map(s_map, old_map);
		} catch (cs::runtime_error& e) {
			if (_debug) std::cout << "  sw mapping failed" << std::endl;
			continue;
		}

		try {
			if (_debug) _os << "link mapping:" << std::endl;
			std::map<unsigned long, unsigned long> sx_map;

			// combine server and switch mappings to new sx_map map
			sx_map.insert(s_map.begin(), s_map.end());
			sx_map.insert(x_map.begin(), x_map.end());
			e_map = _link_map(sx_map);
			break;
		} catch (std::runtime_error& e) {
			if (_debug) _os << "  link mapping failed";
			continue;
		}
	}

	if (s_map.empty() || x_map.empty() || e_map.empty())
		throw cs::runtime_error("embedding failed");

	for (auto s : s_map)
		m.servers.add(s.first, s.second);

	for (auto x : x_map)
		m.switches.add(x.first, x.second);

	for (auto e : e_map)
		m.links.add(e.first, e.second);

	_V.untouch_all_nodes();

	return m;
}

std::map<unsigned long, unsigned long> algo::TAVDCE::_vm_map(std::vector<unsigned long> Sps)
throw (cs::runtime_error)
{
	return _vm_map(Sps, cloudsim::Mapping {});
}

std::map<unsigned long, unsigned long> algo::TAVDCE::_vm_map(std::vector<unsigned long> Sps,
	cloudsim::Mapping cstr)
	throw(cloudsim::runtime_error)
{
	std::vector<unsigned long> old_nodes = {}, new_nodes = {};
	std::map<unsigned long, unsigned long> map {};

	if (_debug) {
		_os << "  Sps:";
		for (unsigned long i : Sps)
			_os << " " << i << "["
				<< dynamic_cast<PhysicalNode*>(*(_P.nodes[i]))->residual(SERVER_CPU) << "]";
		_os << std::endl;
	}

	// sort virtual servers according to req. resource capacity [3]
	std::sort(_Sv.begin(), _Sv.end(), [this](unsigned long a, unsigned long b) {
		double demand_a = dynamic_cast<cs::VirtualNode*>(*(_V.nodes[a]))->demand();
		double demand_b = dynamic_cast<cs::VirtualNode*>(*(_V.nodes[b]))->demand();
		return demand_a > demand_b;
	});


	for (unsigned long i : _Sv)
		(cstr.servers.has(i) ? old_nodes : new_nodes).push_back(i);

	for (unsigned long i : old_nodes) {

		map[i] = cstr.servers[i];

		cs::PhysicalNode* p = dynamic_cast<cs::PhysicalNode*>(*(_P.nodes[cstr.servers[i]]));
		cs::VirtualNode* v = dynamic_cast<cs::VirtualNode*>(*(_V.nodes[i]));

		p->reserve(v->demand(SERVER_CPU), SERVER_CPU);
		p->reserve(v->demand(SERVER_BW), SERVER_BW);
		v->touch();
	}

	for (unsigned long i : new_nodes) {

		BipartiteMatching mcf;

		for (unsigned long j : Sps) { // [6]

			cs::VirtualNode* v = dynamic_cast<cs::VirtualNode*>(*(_V.nodes[i]));
			cs::PhysicalNode* p = dynamic_cast<cs::PhysicalNode*>(*(_P.nodes[j]));

			if (v->demand(SERVER_CPU) <= p->residual(SERVER_CPU)
				&& v->demand(SERVER_BW) <= p->residual(SERVER_BW)) { // [7]
				mcf.add_candidate({(unsigned int) i, (unsigned int) j, _a(i, j)});
			}
		}

		if(mcf.count_candidates() == 0)
			break;

		try {
			// solve min-cost flow [10-14]
			BipartiteMatching::result mcf_result = mcf.find_matching();

			// mcf_result.cost
			for (auto m : mcf_result.matches) {
				if (_debug)
					_os << "      " <<  m.i << " -> " << m.j << std::endl;
				map[m.i] = m.j;
				cs::PhysicalNode* p = dynamic_cast<cs::PhysicalNode*>(*(_P.nodes[m.j]));
				cs::VirtualNode* v = dynamic_cast<cs::VirtualNode*>(*(_V.nodes[m.i]));
				p->reserve(v->demand(SERVER_CPU), SERVER_CPU);
				p->reserve(v->demand(SERVER_BW), SERVER_BW);
				v->touch();
			}

		} catch (std::runtime_error& e) {
			if (_debug)
				std::cout << "no vm matching found: " << e.what() << std::endl;
			break;
		} catch (std::logic_error& e) {
			std::cerr << "Bipartite Matching Error: " << e.what() << std::endl;
			throw cs::runtime_error("no server mapping found");
		}
	}

	// check if every node out of the virtual nodes has been mapped (i.e., touched)
	if (std::all_of(_Sv.begin(), _Sv.end(), [this](unsigned long i) {
		return dynamic_cast<cloudsim::VirtualNode*>(*(_V.nodes[i]))->touched();
	})) {
		return map;
	} else {
		// if no untouch all virtual nodes and reset all reservations on physical nodes

		for (auto& v : _V.vertices) {
			if (dynamic_cast<cloudsim::VirtualNode*>(v)->touched())
				dynamic_cast<cloudsim::VirtualNode*>(v)->untouch();
		}

		for (auto& ap : _P.vertices) {

			cloudsim::PhysicalNode* p = dynamic_cast<cloudsim::PhysicalNode*>(ap);

			if (p->reservation(0) > 0)
				p->reset_reservation(0);

			if (p->reservation(1) > 0)
				p->reset_reservation(1);
		}

		throw cs::runtime_error("no server mapping found");
	}
}


std::map<unsigned long, unsigned long>
algo::TAVDCE::_sw_map(std::map<unsigned long, unsigned long>& vm_map)
throw (cs::runtime_error)
{
	return _sw_map(vm_map, cloudsim::Mapping{});
}

std::map<unsigned long, unsigned long>
	algo::TAVDCE::_sw_map(std::map<unsigned long, unsigned long>& vm_map, cloudsim::Mapping cstr)
	throw(cloudsim::runtime_error)
{
	std::map<unsigned long, unsigned long> map {};

	BipartiteMatching mcf;

	for (unsigned long i : _Xv) { // [18]

		cs::VirtualNode* v = dynamic_cast<cs::VirtualNode*>(*(_V.nodes[i]));

		if (cstr.switches.has(i)) {
			mcf.add_candidate({(unsigned int) i, (unsigned int) cstr.switches[i], 0});
		} else {
			if (_debug)
				_os << "    xv: " << i << "[" << v->demand() << "]" << std::endl;

			for (unsigned long j : _Xp) { // [19]

				cs::PhysicalNode* p = dynamic_cast<cs::PhysicalNode*>(*(_P.nodes[j]));

				if (v->demand() <= p->residual(SWITCH_BW)) // [7]
					mcf.add_candidate({(unsigned int) i, (unsigned int) j, _b(i, j, vm_map)});
			}
		}
	}

	if(mcf.count_candidates() == 0)
		throw cs::runtime_error("no switch mapping found");

	try {
		// solve min-cost flow [24-28]
		BipartiteMatching::result mcf_result = mcf.find_matching();

		// mcf_result.cost
		for (auto m : mcf_result.matches) {
			if (_debug)
				_os << "      " <<  m.i << " -> " << m.j << std::endl;

			cs::VirtualNode* v = dynamic_cast<cs::VirtualNode*>(*(_V.nodes[m.i]));
			cs::PhysicalNode* p = dynamic_cast<cs::PhysicalNode*>(*(_P.nodes[m.j]));

			p->reserve(v->demand(0), 0);
			p->reserve(v->demand(1), 1);
			v->touch();
			map[m.i] = m.j;
		}
	} catch (std::runtime_error& e) { // std::runtime error thrown by Bipartite Matching
		if (_debug)
			_os << "no switch matching found: " << e.what() << std::endl;
		throw cs::runtime_error("no switch mapping found");
	} catch (std::logic_error& e) {
		std::cerr << "Bipartite Matching Error: " << e.what() << std::endl;
		throw cs::runtime_error("no switch mapping found");
	}

	// check if every node out of the virtual nodes has been mapped (i.e., touched)
	if (std::all_of(_Xv.begin(), _Xv.end(), [this](unsigned long i){
		return dynamic_cast<cs::VirtualNode*>(*(_V.nodes[i]))->touched();
	})) {
		return map;
	} else {
		// if no untouch all virtual nodes and reset all reservations on physical nodes

		for (auto& v : _V.vertices) {
			if (dynamic_cast<cloudsim::VirtualNode*>(v)->touched())
				dynamic_cast<cloudsim::VirtualNode*>(v)->untouch();
		}

		for (auto& ap : _P.vertices) {

			cloudsim::PhysicalNode* p = dynamic_cast<cloudsim::PhysicalNode*>(ap);

			if (p->reservation(0) > 0)
				p->reset_reservation(0);

			if (p->reservation(1) > 0)
				p->reset_reservation(1);
		}

		throw cs::runtime_error("no switch mapping found");
	}
}

std::map<unsigned long, std::vector<unsigned long>>
algo::TAVDCE::_link_map(std::map<unsigned long, unsigned long>& map)
throw (cs::runtime_error)
{
	std::map<unsigned long, std::vector<unsigned long>> e_map;

	for (auto i = _V.edges.begin(); i != _V.edges.end(); i++) {

		cs::VirtualEdge* vedge = dynamic_cast<cs::VirtualEdge*>(*i);

		if (map[i.from().id()] != map[i.to().id()]) {

			try {
				auto path = _P.find_path(
					_P.nodes[map[i.from().id()]], _P.nodes[map[i.to().id()]],
					[this, &vedge] (cs::Edge** e) {
						return dynamic_cast<cs::PhysicalEdge*>(*e)->residual(LINK_BW) >= vedge->demand();
					});

				e_map[i.id()] = {};

				for (auto j : path) {
					e_map[i.id()].push_back(j.id());
					dynamic_cast<cs::PhysicalEdge*>(*j)->reserve(vedge->demand(), LINK_BW);
				}

				if (_debug) {
					std::cout << "      " << i.id() << " ->";
					for (auto e : e_map[i.id()]) std::cout << " " << e;
					std::cout << std::endl;
				}

			} catch(plexum::Graph<cloudsim::Node*, cloudsim::Edge*>::exception& e) {
				_V.untouch_all_nodes();
				_P.reset_node_reservations();
				_P.reset_edge_reservations();
				throw cs::runtime_error("TAVDCE: edge mapping failed.");
			}
		}
	}

	return e_map;
}

double algo::TAVDCE::_a(unsigned long i, unsigned long j) const
{
	cs::PhysicalNode* p = dynamic_cast<cs::PhysicalNode*>(*(_P.nodes[j]));
	int lb = std::abs((int) p->residual(0) - (int) _P.avg_node_residual(0, 1));
	return p->utilization(SERVER_BW) * ( 1 / (lb == 0 ? 1 : lb));
}

double algo::TAVDCE::_mean_distance_to_connected_servers(unsigned long i, unsigned long j,
										   std::map<unsigned long, unsigned long>& s_map) const
{
	std::vector<unsigned long> servers = {};
	std::vector<int> distance = {};

	for(auto n : _V.nodes[i].neighbors())
		if (dynamic_cast<VirtualNode*>(*n)->type() == TYPE_SERVER)
			servers.push_back(n.id());

	if (servers.size() == 0)
		return 0;

	for (auto k : servers)
		distance.push_back(_d[j][s_map[k]]);

	return std::accumulate(distance.begin(), distance.end(), 0) / distance.size();
}

double algo::TAVDCE::_b(unsigned long i, unsigned long j,
									 std::map<unsigned long, unsigned long>& s_map) const
{
	double cost = 0;
	/*
	VirtualNode*  v = dynamic_cast<VirtualNode*>(*(_V.nodes[i]));
	PhysicalNode* p = dynamic_cast<PhysicalNode*>(*(_P.nodes[j]));

	if (_V.type() == 0) { // VC
		if (_V.total_node_demand(1, 0) > 10000) {
			if (p->group() != 0)
				cost += 2;
		} else {
			if (p->group() == 0)
				cost += 2;
		}

	} else if (_V.type() == 1) { // VOC

		cost += 0;

	} else if (_V.type() == 2) { // 3T

		cost += 0;
	}
	*/
	cost += _mean_distance_to_connected_servers(i, j, s_map);

	return cost;
}
