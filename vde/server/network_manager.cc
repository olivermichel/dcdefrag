
#include <cloudsim/greedy.h>
#include <json/json.h>
#include <fstream>
#include <cloudsim/util.h>
#include "../algo/tavdce.h"

#include "network_manager.h"
#include "../algo/rabbani_vdce/vdce0.h"
#include "../util/util.h"

NetworkManager::data_point::data_point(
	NetworkManager::op_type type, unsigned long net_id, bool accept, double time,
	cloudsim::PhysicalNetwork* p)
	: type(type),
	  net_id(net_id),
	  accept(accept),
	  time(time),
	  rel_server_utilization(p->rel_node_utilization(0, 1)),
 	  rel_switch_utilization(p->rel_node_utilization(0, 0)),
	  rel_link_utilization(p->rel_edge_utilization(0)),
	  rel_server_utilization_variance(p->rel_node_utilization_variance(0, 1)),
	  rel_switch_utilization_variance(p->rel_node_utilization_variance(0, 0)),
	  rel_link_utilization_variance(p->rel_edge_utilization_variance(0)),
	  vn_count(p->subgraphs().size()),
	  path_length_distribution(),
	  rel_server_utilization_distribution(p->rel_node_utilization_distribution(0, 0)),
	  rel_switch_utilization_distribution(p->rel_node_utilization_distribution(0, 1)),
	  rel_link_utilization_distribution(p->rel_edge_utilization_distribution(0))
{
	std::vector<std::vector<double>> path_length;

	for (auto sub : p->subgraphs()) {
		path_length.push_back(
			static_cast<cloudsim::VirtualNetwork*>(sub)->last_mapping().path_length_distribution()
		);
	}

	path_length_distribution = path_length;
}

nlohmann::json NetworkManager::data_point::to_json() const
{
	nlohmann::json j;

	j["type"]                                 = (unsigned long) type;
	j["net_id"]                               = net_id;
	j["accept"]                               = accept;
	j["time"]                                 = time;
	j["rel_server_utilization"]               = rel_server_utilization;
	j["rel_switch_utilization"]               = rel_switch_utilization;
	j["rel_link_utilization"]                 = rel_link_utilization;
	j["rel_server_utilization_variance"]      = rel_server_utilization_variance;
	j["rel_switch_utilization_variance"]      = rel_switch_utilization_variance;
	j["rel_link_utilization_variance"]        = rel_link_utilization_variance;
	j["vn_count"]                             = vn_count;
	j["path_length_distribution"]             = path_length_distribution;
	j["rel_link_utilization_distribution"]    = rel_link_utilization_distribution;
	j["rel_server_utilization_distribution"]  = rel_server_utilization_distribution;
	j["rel_switch_utilization_distribution"]  = rel_switch_utilization_distribution;

	return j;
}

bool NetworkManager::statistics::empty() const
{
	return data.empty();
}

const NetworkManager::data_point& NetworkManager::statistics::last()
	throw(std::logic_error)
{
	if (empty())
		throw std::logic_error("NetworkManager::statistics::last(): stats is empty");

	return data.back();
}

json NetworkManager::statistics::to_json() const
{
	json j {};
	for (data_point p : data)
		j.push_back(p.to_json());
	return j;
}

void NetworkManager::statistics::write_to_file(std::string file_name)
{
	std::ofstream o(file_name);
	o << /* std::setw(2) << */ to_json() << std::endl;
	o.close();
}

NetworkManager::NetworkManager(cloudsim::PhysicalNetwork* p)
	: _physical_network(p),
	  _distance_matrix(p->distance_matrix()),
	  _counter(0),
	  _stats(),
	  _request_running(false)
{
	std::cout << "[NetworkManager] initialized" << std::endl;
}

NetworkManager::NetworkManager(cloudsim::PhysicalNetwork* p, std::string out_dir)
	: _physical_network(p),
	  _distance_matrix(p->distance_matrix()),
	  _counter(0),
	  _stats(),
	  _request_running(false)
{
	std::cout << "[NetworkManager] initialized" << std::endl;
}

cloudsim::PhysicalNetwork* NetworkManager::physical_network() const
{
	return _physical_network;
}

bool NetworkManager::map_virtual_network(std::unique_ptr<cloudsim::VirtualNetwork> v)
{
	bool accept = false;
	double time = 0, path_len = -1;
	unsigned long network_id = v->id();

	if (_request_running) {
		std::cerr << "[NetworkManager] request running" << std::endl;
		return false;
	}

	_request_running = true;

	try {
		algo::TAVDCE tavdce(*_physical_network, *v, _distance_matrix, false, std::cout);
		cloudsim::Mapping m;
		std::cout << "[NetworkManager] initialized TAVDCE: r=" << v->id() << ", n = "
				  << v->nodes.count() << ", m=" << v->edges.count() << std::endl;
		time = util::duration([&m, &tavdce](){ m = tavdce(); });
		std::cout << "[NetworkManager] TAVDCE " << v->id() << " successful: took "
				  << time << "s" << std::endl;

		_physical_network->add_virtual_network(v.get(), m);
		_request_running = false;

		accept = true;
		_networks.push_back(std::move(v));
	} catch (cloudsim::logic_error& e) {
		std::cerr << "[NetworkManager] " << v->id() << " failed: " << e.what() << std::endl;
		_request_running = false;
		accept = false, time = -1, path_len = -1;
	} catch (cloudsim::runtime_error& e) {
		std::cerr << "[NetworkManager] " << v->id() << " failed: " << e.what() << std::endl;
		_request_running = false;
		accept = false, time = -1, path_len = -1;
 	}

	data_point p(op_type::map, network_id, accept, time, _physical_network);
	_stats.data.push_back(p);

	return accept;
}

bool NetworkManager::expand_virtual_network(std::unique_ptr<cloudsim::VirtualNetwork>& v,
	unsigned long a)
{
	if (_request_running) {
		std::cerr << "[NetworkManager] request running" << std::endl;
		return false;
	}

	std::cout << "[NetworkManager] expand virtual network " << v->id() << std::endl;

	bool accept = false;
	double time = 0, path_len = -1;
	_request_running = true;

	try {
		_physical_network->remove_virtual_network(v.get());
		std::cout << "[NetworkManager] " << v->id() << " deleted" << std::endl;
	} catch (cloudsim::runtime_error& e) {
		std::cerr << "[NetworkManager] " << v->id() << " failed: " << e.what() << std::endl;
		_request_running = false;
		return false;
	} catch (cloudsim::logic_error& e) {
		std::cerr << "[NetworkManager] " << v->id() << " failed: " << e.what() << std::endl;
		_request_running = false;
		return false;
	} catch (...) {
		std::cerr << "[NetworkManager] " << v->id() << " failed." << std::endl;
		_request_running = false;
		return false;
	}
	//TODO: remove additional error handling

	using node_iter = plexum::Graph<cloudsim::Node*, cloudsim::Edge*>::vertex_proxy::iterator;
	using edge_iter = plexum::Graph<cloudsim::Node*, cloudsim::Edge*>::edge_proxy::iterator;

	unsigned long n = v->nodes.count(), m = v->edges.count();
	std::vector<node_iter> new_nodes;
	std::vector<edge_iter> new_edges;

	node_iter r = v->nodes[0], k = v->nodes.end();
	edge_iter l = v->edges.end();

	for (size_t i = 0; i < a; i++) {
		k = v->nodes.add(new cloudsim::VirtualNode{n + i, {100, 100}, 1, 0, v.get()});
		l = v->edges.add(r, k, new cloudsim::VirtualEdge{m + i , 100, 0, v.get()});
		new_nodes.push_back(k);
		new_edges.push_back(l);
	}

	cloudsim::Mapping old_map = v->last_mapping();

	try {
		algo::TAVDCE tavdce(*_physical_network, *v, _distance_matrix);
		cloudsim::Mapping new_map;
		std::cout << "[NetworkManager] initialized VDCE-expand: r=" << v->id() << ", n = "
				  << v->nodes.count() << ", m=" << v->edges.count() << std::endl;
		time = util::duration([&new_map, &tavdce, &old_map]() { new_map = tavdce(old_map); });
		std::cout << "[NetworkManager] VDCE-expand r=" << v->id() << " successful: took "
				  << time << "s" << std::endl;

		_physical_network->add_virtual_network(v.get(), new_map);

		accept = true;

	} catch (cloudsim::logic_error& e) {
		std::cerr << "[NetworkManager] (logic) VDCE-expand r=" << v->id() << " failed: "
				  << e.what() << std::endl;
		std::cerr << "[NetworkManager] discard request" << std::endl;
		_request_running = false;
		accept = false, time = -1;

		for (auto i : new_edges)
			v->links.remove(i);

		for (auto i : new_nodes)
			v->nodes.remove(i);

	} catch (cloudsim::runtime_error& e) {

		std::cerr << "[NetworkManager] (runtime) VDCE-expand r=" << v->id() << " failed: "
				  << e.what() << std::endl;
		_request_running = false;
		accept = false, time = -1;

		for (auto i : new_edges)
			v->links.remove(i);

		for (auto i : new_nodes)
			v->nodes.remove(i);

		_physical_network->add_virtual_network(v.get(), old_map);
	}

	data_point p(op_type::expand, v->id(), true, time, _physical_network);
	_stats.data.push_back(p);
	_request_running = false;

	return accept;
}

void NetworkManager::contract_virtual_network(std::unique_ptr<cloudsim::VirtualNetwork>& v,
	unsigned long a)
{
	return;
}

void NetworkManager::unmap_virtual_network(std::unique_ptr<cloudsim::VirtualNetwork>& v)
{
	if (_request_running) {
		std::cerr << "[NetworkManager] request running" << std::endl;
		return;
	}

	_request_running = true;

	try {
		_physical_network->remove_virtual_network(v.get());
		std::cout << "[NetworkManager] " << v->id() << " deleted" << std::endl;
	} catch (cloudsim::runtime_error& e) {
		std::cerr << "[NetworkManager] " << v->id() << " failed: " << e.what() << std::endl;
		_request_running = false;
	} catch (cloudsim::logic_error& e) {
		std::cerr << "[NetworkManager] " << v->id() << " failed: " << e.what() << std::endl;
		_request_running = false;
	} catch (...) {
		std::cerr << "[NetworkManager] " << v->id() << " failed." << std::endl;
		_request_running = false;
	}

	data_point p(op_type::unmap, v->id(), true, -1, _physical_network);
	_stats.data.push_back(p);
	_networks.erase(std::find(_networks.begin(), _networks.end(), v));
	_request_running = false;
}

bool NetworkManager::virtual_network_exists(unsigned long r)
{
	return std::find_if(std::begin(_networks), std::end(_networks),
		[&r](std::unique_ptr<cloudsim::VirtualNetwork>& v) {
			return v->id() == r;
		}) != std::end(_networks);
}

std::unique_ptr<cloudsim::VirtualNetwork>&
	NetworkManager::random_virtual_network(std::random_device& r)
	throw(std::logic_error)
{
	if (_networks.empty())
		throw std::logic_error("NetworkManager::random_network(): no networks");

	std::vector<std::unique_ptr<cloudsim::VirtualNetwork>>::iterator v = std::begin(_networks);
	std::advance(v, std::uniform_int_distribution<>(0, (int)(_networks.size() - 1))(r));
	return *v;
}

std::unique_ptr<cloudsim::VirtualNetwork>&  NetworkManager::virtual_network_by_id(unsigned long r)
	throw(std::logic_error)
{
	std::vector<std::unique_ptr<cloudsim::VirtualNetwork>>::iterator i = std::find_if(
		std::begin(_networks), std::end(_networks),
		    [&r](std::unique_ptr<cloudsim::VirtualNetwork>& v) -> bool {
			return v->id() == r;
		});

	if (i == std::end(_networks))
		throw std::logic_error("NetworkManager::network_by_id(): does not exist");

	return *i;
}

std::map<unsigned long, bool> NetworkManager::remap()
{
//	std::vector<bool> result;

	std::map<unsigned long, bool> result;

	if (_request_running) {
		std::cerr << "[NetworkManager] request running" << std::endl;
		return result;
	}

	_request_running = true;

	std::vector<cloudsim::VirtualNetwork*> remap_candidates;

	unsigned long n = _networks.size();
	const float take = 1;
	int m = (int) floor(n * take);

	std::cout << "[NetworkManager] remap " << m << " networks" << std::endl;

	std::sort(_networks.begin(), _networks.end(),
		[] (std::unique_ptr<cloudsim::VirtualNetwork>& a,
			std::unique_ptr<cloudsim::VirtualNetwork>& b) -> bool {
		return a->nodes.count() < b->nodes.count();
	});

	for (auto i = _networks.begin(); i != _networks.begin() + m; i++)
		remap_candidates.push_back(i->get());

	for (auto v : remap_candidates) {
		_physical_network->remove_virtual_network(v);
		std::cout << "[NetworkManager] removed network r=" << v->id() << std::endl;
	}

	std::sort(remap_candidates.begin(), remap_candidates.end(),
		[] (cloudsim::VirtualNetwork* a, cloudsim::VirtualNetwork* b) -> bool {
		  return a->nodes.count() > b->nodes.count();
		});

	for (auto v : remap_candidates) {

		try {
			algo::TAVDCE tavdce(*_physical_network, *v, _distance_matrix, false, std::cout);
			cloudsim::Mapping new_map;
			std::cout << "[NetworkManager] initialized VDCE: r=" << v->id() << ", n="
					  << v->nodes.count() << ", m=" << v->edges.count() << std::endl;
			double t = util::duration([&new_map, &tavdce](){ new_map = tavdce(); });
			std::cout << "[NetworkManager] VDCE " << v->id() << " successful: took " << t << "s"
					  << std::endl;

			result[v->id()] = true;

			_physical_network->add_virtual_network(v, new_map);

		} catch (const cloudsim::logic_error& e) {
			std::cerr << "[NetworkManager] (logic) VDCE r=" << v->id() << " failed: "
					  << e.what() << std::endl;
			std::cerr << "[NetworkManager] discard request" << std::endl;
			result[v->id()] = false;
		} catch (const cloudsim::runtime_error& e) {
			std::cerr << "[NetworkManager] (runtime) VDCE r=" << v->id() << " failed: "
					  << e.what() << std::endl;
			std::cerr << "[NetworkManager] discard request" << std::endl;
			result[v->id()] = false;
		}
	}

	for (auto& p : result) {
		if (!p.second) {
			_networks.erase(std::find_if(std::begin(_networks), std::end(_networks),
				[&p] (std::unique_ptr<cloudsim::VirtualNetwork>& v) -> bool {
					return v->id() == p.first;
				}));
			std::cout << "[NetworkManager] removed network " << p.first << std::endl;
		}
	}

	std::cout << "[NetworkManager] remap done" << std::endl;
	_request_running = false;

	return result;
}

void NetworkManager::save_configuration(std::string file_name)
{
	using json = nlohmann::json;

	std::ofstream of(file_name);
	json j;

	j["networks"] = json::array();

	for (auto abstract_vnet : _physical_network->subgraphs()) {

		cloudsim::VirtualNetwork* vnet = static_cast<cloudsim::VirtualNetwork*>(abstract_vnet);

		json vnet_obj {
			{"id", vnet->id() },
			{"topo", vnet->_to_json() },
			{"map", vnet->last_mapping()._to_json() }
		};

		j["networks"].push_back(vnet_obj);
	}

	of << j << std::endl;
}

void NetworkManager::load_configuration(std::string file_name)
{
	using json = nlohmann::json;
	std::ifstream fs(file_name);
	json j;

	if (fs.is_open())
		fs >> j, fs.close();
	else
		throw std::runtime_error("couldn't open file");

	for (auto n : j["networks"]) {
		cloudsim::Mapping m;
		m._from_json(n["map"]);
		std::unique_ptr<cloudsim::VirtualNetwork> v(new cloudsim::VirtualNetwork());
		std::cout << "[NetworkManager] load network " <<  n["id"].get<unsigned long>() << std::endl;
		v->_from_json(n["topo"]);
		_physical_network->add_virtual_network(v.get(), m);
		_networks.push_back(std::move(v));
	}
}

struct NetworkManager::statistics& NetworkManager::statistics()
{
	return _stats;
}
