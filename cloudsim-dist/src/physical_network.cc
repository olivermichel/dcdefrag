
#include "cloudsim/physical_network.h"

#include "cloudsim/physical_node.h"
#include "cloudsim/physical_edge.h"
#include <fstream>
#include <json/json.h>
#include <random>
#include "cloudsim/util.h"

cloudsim::PhysicalNetwork::PhysicalNetwork()
	: plexum::Graph<Node*, Edge*>(),
	  nodes(vertices),
	  links(edges),
	  _distance_matrix(),
	  _diameter(-1)
{ }

cloudsim::PhysicalNetwork::PhysicalNetwork(std::string file_name) throw (std::runtime_error)
	: plexum::Graph<Node*, Edge*>(),
	  nodes(vertices),
	  links(edges),
	  _distance_matrix(),
	  _diameter(-1)
{
	_read_from(file_name);
}

cloudsim::PhysicalNetwork::PhysicalNetwork(PhysicalNetwork& copy_from)
	: plexum::Graph<Node*, Edge*>(copy_from),
	  nodes(copy_from.vertices),
	  links(copy_from.edges),
	  _distance_matrix(copy_from._distance_matrix),
	  _diameter(copy_from._diameter)
{ }

void cloudsim::PhysicalNetwork::reset_node_reservations()
{
	for (auto abstract_node : nodes) {
		PhysicalNode* n = dynamic_cast<PhysicalNode*>(abstract_node);
		if (n->reservation() > 0) n->reset_reservation();
	}
}

void cloudsim::PhysicalNetwork::block_node_reservations()
{
	for (auto abstract_node : nodes) {
		PhysicalNode* n = dynamic_cast<PhysicalNode*>(abstract_node);
		if (n->reservation() > 0) n->block(n->reservation());
	}
}

void cloudsim::PhysicalNetwork::reset_edge_reservations()
{
	for (auto abstract_edge : edges) {
		PhysicalEdge* e = dynamic_cast<PhysicalEdge*>(abstract_edge);
		if (e->reservation() > 0) e->reset_reservation();
	}
}

void cloudsim::PhysicalNetwork::block_edge_reservations()
{
	for (auto abstract_edge : edges) {
		PhysicalEdge* e = dynamic_cast<PhysicalEdge*>(abstract_edge);
		if (e->reservation() > 0) e->block(e->reservation());
	}
}
/*
template<typename T>
std::vector<T>
cloudsim::PhysicalNetwork::node_distribution(std::function<T (cloudsim::PhysicalNode*)> f)
{
	std::vector<T> v;
	std::for_each(nodes.begin(), nodes.end(), [&v, &f] (Node* e) {
		v.push_back(f(dynamic_cast<cloudsim::PhysicalNode*>(e)));
	});
	return v;
}

template<typename T>
std::vector<T> cloudsim::PhysicalNetwork::node_distribution(
	std::function<T (cloudsim::PhysicalNode*)> f, unsigned long type)
{
	std::vector<T> v;
	std::for_each(nodes.begin(), nodes.end(), [&v, &f, &type] (Node* e) {
		if (type == 0 || e->type() == type)
			v.push_back(f(dynamic_cast<cloudsim::PhysicalNode*>(e)));
	});
	return v;
};

template<typename T>
std::vector<T>
cloudsim::PhysicalNetwork::edge_distribution(std::function<T (cloudsim::PhysicalEdge*)> f)
{
	std::vector<T> v;
	std::for_each(edges.begin(), edges.end(), [&v, &f] (Edge* e) {
		v.push_back(f(dynamic_cast<cloudsim::PhysicalEdge*>(e)));
	});
	return v;
};
*/

unsigned long cloudsim::PhysicalNetwork::count_nodes(unsigned long node_type)
{
	return std::accumulate(nodes.begin(), nodes.end(), (unsigned long) 0,
		[&node_type](unsigned long& a, Node* n) -> unsigned long {
			return a += n->type() == node_type ? 1 : 0;
	});
}

unsigned cloudsim::PhysicalNetwork::total_node_capacity(unsigned long resource_type)
{
	return (unsigned int) std::accumulate(nodes.begin(), nodes.end(), 0,
										  [&resource_type](int& a, Node* n) -> int {
			   return a += dynamic_cast<cloudsim::PhysicalNode*>(n)->capacity(resource_type);
		});
}

unsigned cloudsim::PhysicalNetwork::total_node_capacity(unsigned long resource_type,
													  unsigned long node_type)
{
	return (unsigned int) std::accumulate(nodes.begin(), nodes.end(), 0,
	   [&resource_type, &node_type](int& a, Node* n) -> int {
		   return n->type() == node_type ?
			   a += dynamic_cast<cloudsim::PhysicalNode*>(n)->capacity(resource_type) : a;
	});
}

unsigned cloudsim::PhysicalNetwork::total_edge_capacity(unsigned long resource_type)
{
	return (unsigned int) std::accumulate(edges.begin(), edges.end(), 0,
	   [&resource_type](int& a, Edge* e) -> int {
		   return a += dynamic_cast<PhysicalEdge*>(e)->capacity(resource_type);
	});
}

unsigned cloudsim::PhysicalNetwork::total_node_utilization(unsigned long resource_type)
{
	return (unsigned int) std::accumulate(nodes.begin(), nodes.end(), 0,
		[&resource_type](int& a, Node* n) -> int {
			return a += dynamic_cast<cloudsim::PhysicalNode*>(n)->utilization(resource_type);
	});
}

unsigned cloudsim::PhysicalNetwork::total_node_utilization(unsigned long resource_type,
														 unsigned long node_type)
{
	return  (unsigned int) std::accumulate(nodes.begin(), nodes.end(), 0,
		[&resource_type, &node_type](int& a, Node* n) -> int {
		return n->type() == node_type ?
			a += dynamic_cast<cloudsim::PhysicalNode*>(n)->utilization(resource_type) : a;
	});
}

unsigned cloudsim::PhysicalNetwork::total_edge_utilization(unsigned long resource_type)
{
	return (unsigned int)  std::accumulate(edges.begin(), edges.end(), 0,
		[&resource_type](int& a, Edge* e) -> int {
		return a += dynamic_cast<PhysicalEdge*>(e)->utilization(resource_type);
	});
}

double cloudsim::PhysicalNetwork::rel_node_utilization(unsigned long resource_type)
{
	return (double) total_node_utilization(resource_type)
		   / (double) total_node_capacity(resource_type);
}

double cloudsim::PhysicalNetwork::rel_node_utilization(unsigned long resource_type,
													   unsigned long node_type)
{
	return (double) total_node_utilization(resource_type, node_type)
		   / (double) total_node_capacity(resource_type, node_type);
}

double cloudsim::PhysicalNetwork::rel_edge_utilization(unsigned long resource_type)
{
	return (double) total_edge_utilization(resource_type)
		   / (double) total_edge_capacity(resource_type);
}

double cloudsim::PhysicalNetwork::avg_node_utilization(unsigned long resource_type)
{
	return total_node_utilization(resource_type) / nodes.count();
}

double cloudsim::PhysicalNetwork::avg_node_utilization(unsigned long resource_type,
													   unsigned long node_type)
{
	return total_node_utilization(resource_type, node_type) / count_nodes(node_type);
}

double cloudsim::PhysicalNetwork::avg_edge_utilization(unsigned long resource_type)
{
	return total_edge_utilization(resource_type) / edges.count();
}

unsigned cloudsim::PhysicalNetwork::total_node_residual(unsigned long resource_type)
{
	return  (unsigned int) std::accumulate(nodes.begin(), nodes.end(), 0,
	   [&resource_type](int& a, Node* n) -> int {
		   return a += dynamic_cast<cloudsim::PhysicalNode*>(n)->residual(resource_type);
	   });
}

unsigned cloudsim::PhysicalNetwork::total_node_residual(unsigned long resource_type,
													  unsigned long node_type)
{
	return  (unsigned int) std::accumulate(nodes.begin(), nodes.end(), 0,
	   [&resource_type, &node_type](int& a, Node* n) -> int {
		   return n->type() == node_type ?
			   a += dynamic_cast<cloudsim::PhysicalNode*>(n)->residual(resource_type) : a;
	   });
}

unsigned cloudsim::PhysicalNetwork::total_edge_residual(unsigned long resource_type)
{
	return  (unsigned int) std::accumulate(edges.begin(), edges.end(), 0,
	   [&resource_type](int& a, Edge* e) -> int {
		   return a += dynamic_cast<PhysicalEdge*>(e)->residual(resource_type);
	   });
}

double cloudsim::PhysicalNetwork::avg_node_residual(unsigned long resource_type)
{
	return total_node_residual(resource_type) / nodes.count();
}

double cloudsim::PhysicalNetwork::avg_node_residual(unsigned long resource_type,
													unsigned long node_type)
{
	return total_node_residual(resource_type, node_type) / count_nodes(node_type);
}

double cloudsim::PhysicalNetwork::avg_edge_residual(unsigned long resource_type)
{
	return total_edge_residual(resource_type) / edges.count();
}

double cloudsim::PhysicalNetwork::node_utilization_variance(unsigned long resource_type)
{
	return cloudsim::util::variance<unsigned>(node_utilization_distribution(resource_type));
}

double cloudsim::PhysicalNetwork::node_utilization_variance(unsigned long resource_type,
															unsigned long node_type)
{
	return util::variance<unsigned>(node_utilization_distribution(resource_type, node_type));
}

double cloudsim::PhysicalNetwork::edge_utilization_variance(unsigned long resource_type)
{
	return util::variance<unsigned>(edge_utilization_distribution(resource_type));
}

double cloudsim::PhysicalNetwork::rel_node_utilization_variance(unsigned long resource_type)
{
	return cloudsim::util::variance<double>(rel_node_utilization_distribution(resource_type));
}

double cloudsim::PhysicalNetwork::rel_node_utilization_variance(unsigned long resource_type,
																unsigned long node_type)
{
	return util::variance<double>(rel_node_utilization_distribution(resource_type, node_type));
}

double cloudsim::PhysicalNetwork::rel_edge_utilization_variance(unsigned long resource_type)
{
	return util::variance<double>(rel_edge_utilization_distribution(resource_type));
}

std::vector<unsigned> cloudsim::PhysicalNetwork::node_utilization_distribution(
	unsigned long resource_type)
{
	std::vector<unsigned> d;

	for (auto node : nodes)
		d.push_back(dynamic_cast<cloudsim::PhysicalNode*>(node)->utilization(resource_type));

	return d;
}

std::vector<unsigned> cloudsim::PhysicalNetwork::node_utilization_distribution(
	unsigned long resource_type, unsigned long node_type)
{
	std::vector<unsigned> d;

	for (auto node : nodes)
		if (node->type() == node_type)
			d.push_back(dynamic_cast<cloudsim::PhysicalNode*>(node)->utilization(resource_type));

	return d;
}

std::vector<unsigned> cloudsim::PhysicalNetwork::edge_utilization_distribution(
	unsigned long resource_type)
{
	std::vector<unsigned> d;

	for (auto edge : edges)
		d.push_back(dynamic_cast<cloudsim::PhysicalEdge*>(edge)->utilization());

	return d;
}

/*
std::vector<double>
cloudsim::PhysicalNetwork::node_residual_distribution(unsigned long resource_type)
{
	return {};
}

std::vector<double>
cloudsim::PhysicalNetwork::
node_residual_distribution(unsigned long resource_type, unsigned long node_type)
{
	return {};
}

std::vector<double>
cloudsim::PhysicalNetwork::edge_residual_distribution(unsigned long resource_type)
{
	return {};
}
*/

std::vector<double>
cloudsim::PhysicalNetwork::rel_node_utilization_distribution(unsigned long resource_type)
{
	std::vector<double> d;

	for (auto node : nodes) {
		cloudsim::PhysicalNode* p_node = dynamic_cast<cloudsim::PhysicalNode*>(node);
		d.push_back((double) p_node->utilization(resource_type)
					/ (double) p_node->capacity(resource_type));
	}

	return d;
}

std::vector<double> cloudsim::PhysicalNetwork::rel_node_utilization_distribution(
	unsigned long resource_type, unsigned long node_type)
{
	std::vector<double> d;

	for (auto node : nodes) {
		if (node->type() == node_type) {
			cloudsim::PhysicalNode* p_node = dynamic_cast<cloudsim::PhysicalNode*>(node);
			d.push_back( (double) p_node->utilization(resource_type)
						 / (double) p_node->capacity(resource_type));
		}
	}

	return d;
}

std::vector<double> cloudsim::PhysicalNetwork::rel_edge_utilization_distribution(
	unsigned long resource_type)
{
	std::vector<double> d;

	for (auto edge : edges) {
		cloudsim::PhysicalEdge* p_edge = dynamic_cast<cloudsim::PhysicalEdge*>(edge);
		d.push_back((double) p_edge->utilization(resource_type)
					/ (double) p_edge->capacity(resource_type));
	}

	return d;
}

void cloudsim::PhysicalNetwork::add_virtual_network(cloudsim::VirtualNetwork* v, cloudsim::Mapping& m)
{
	map(v);

	for (auto s : m.servers) {
		cloudsim::PhysicalNode* pn
			= dynamic_cast<cloudsim::PhysicalNode*>(*(nodes[s.second]));
		cloudsim::VirtualNode*  vn = dynamic_cast<cloudsim::VirtualNode*>(*(v->nodes[s.first]));

		try {
			pn->block(vn->demand(0), 0);
			pn->block(vn->demand(1), 1);
			nodes[s.second].map(v->nodes[s.first]);
		} catch (...) {

			std::cerr << "PhysicalNetwork::add_virtual_network: server mapping failed:" << std::endl;
			std::cerr << "  " << s.first << " -> " << s.second << ": "
					  << pn->residual(0) << "/" << pn->residual(1) << " (" << pn->reservation(0)
					  << "/" << pn->reservation(1) << ") - " << vn->demand(0) << "/"
					  << vn->demand(1) << std::endl;
			throw;
		}
	}

	for (auto s : m.switches) {
		cloudsim::PhysicalNode* pn = dynamic_cast<cloudsim::PhysicalNode*>(*(nodes[s.second]));
		cloudsim::VirtualNode*  vn = dynamic_cast<cloudsim::VirtualNode*>(*(v->nodes[s.first]));

		try {
			pn->block(vn->demand(0), 0);
			pn->block(vn->demand(1), 1);
			nodes[s.second].map(v->nodes[s.first]);
		} catch (...) {
			std::cerr << "PhysicalNetwork::add_virtual_network: switch mapping failed:" << std::endl;
			std::cerr << "  " << s.first << " -> " << s.second << ": "
					  << pn->residual(0) << "/" << pn->residual(1) << " (" << pn->reservation(0)
					  << "/" << pn->reservation(1) << ") - " << vn->demand(0) << "/"
					  << vn->demand(1) << std::endl;
			throw;
		}
	}

	for (auto s : m.links) {
		cloudsim::VirtualEdge* ve = dynamic_cast<cloudsim::VirtualEdge*>(*(v->edges[s.first]));

		for (auto m : s.second) {

			cloudsim::PhysicalEdge* pe
				= dynamic_cast<cloudsim::PhysicalEdge*>(*(edges[m]));

			try {
				edges[m].map_path(v->edges[s.first]);
				pe->block(ve->demand(0), 0);
			} catch (...) {
				std::cerr << "PhysicalNetwork::add_virtual_network: link mapping failed:" << std::endl;
				std::cerr << "  " << s.first << " -> " << m << " "
						  << pe->residual(0) << " (" << pe->reservation(0) << ") - "
						  << ve->demand(0) << std::endl;
				throw;
			}
		}
	}

	v->add_mapping(m);
}

void cloudsim::PhysicalNetwork::remove_virtual_network(cloudsim::VirtualNetwork* v)
{
	cloudsim::Mapping m = v->last_mapping();

	try {

		for (auto i : m.servers) {
			std::vector<cloudsim::Node**> subvertices = nodes[i.second].sub_vertices();
			nodes[i.second].unmap(v->nodes[i.first]);
			auto vn = dynamic_cast<cloudsim::VirtualNode*>(*(v->nodes[i.first]));
			auto pn = dynamic_cast<cloudsim::PhysicalNode*>(*(nodes[i.second]));
			pn->release(vn->demand(0), 0);
			pn->release(vn->demand(1), 1);
		}

		for (auto i : m.switches) {
			nodes[i.second].unmap(v->nodes[i.first]);
			auto vn = dynamic_cast<cloudsim::VirtualNode*>(*(v->nodes[i.first]));
			auto pn = dynamic_cast<cloudsim::PhysicalNode*>(*(nodes[i.second]));
			pn->release(vn->demand(0), 0);
			pn->release(vn->demand(1), 1);
		}

		for (auto i : m.links) {

			auto ve = dynamic_cast<cloudsim::VirtualEdge*>(*(v->edges[i.first]));

			for (auto j : i.second) {
				edges[j].unmap(v->edges[i.first]);
				auto pe = dynamic_cast<cloudsim::PhysicalEdge*>(*(edges[j]));
				pe->release(ve->demand(0), 0);
			}
		}

		unmap(v);

	} catch(plexum::Graph<Node*, Edge*>::exception& e) {
		std::cerr << "caught plexum::Graph<Node*, Edge*>::exception: " << e.what() << std::endl;
		throw cloudsim::logic_error("forward plexum::Graph<Node*, Edge*>::exception: " + std::string(e.what()));
	}
}

cloudsim::VirtualNetwork* cloudsim::PhysicalNetwork::virtual_network(unsigned long id)
	throw (cloudsim::logic_error)
{
	for (auto i : this->subgraphs()) {

		auto v = static_cast<cloudsim::VirtualNetwork*>(i);

		if (v->id() == id)
			return v;
	}

	throw logic_error("network does not exist");
}

cloudsim::VirtualNetwork* cloudsim::PhysicalNetwork::random_virtual_network()
	throw (cloudsim::logic_error)
{
	if (this->subgraphs().size() == 0)
		throw logic_error("no networks");

	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_int_distribution<> distribution(0, (int) this->subgraphs().size());
	return static_cast<cloudsim::VirtualNetwork*>(this->subgraphs()[distribution(generator)]);
}

std::vector<cloudsim::VirtualNetwork*> cloudsim::PhysicalNetwork::virtual_networks()
{
	std::vector<VirtualNetwork*> v;

	for (auto i : this->subgraphs())
		v.push_back(static_cast<cloudsim::VirtualNetwork*>(i));

	return v;
}

std::vector<double> cloudsim::PhysicalNetwork::avg_path_length_distribution()
{
	std::vector<double> v;

	for (auto i : virtual_networks())
		v.push_back(i->last_mapping().avg_path_length() / diameter());

	return v;
}

double cloudsim::PhysicalNetwork::avg_path_length()
{
	std::vector<double> path_len = avg_path_length_distribution();
	return std::accumulate(path_len.begin(), path_len.end(), 0.0) / path_len.size();
}

std::vector<std::vector<int>> cloudsim::PhysicalNetwork::distance_matrix()
{
	// implements Floyd-Warshall all pairs shortest-path
	// cp. Cormen, Leiserson, Rivest, Stein. Intro to Algorithms 3rd Ed., pp. 693

	const int INF = 999999;
	std::vector<std::vector<int>> d(nodes.count(), std::vector<int>(nodes.count(), INF));

	for (unsigned i = 0; i < nodes.count(); i++)
		d[i][i] = 0;

	for (auto i = edges.begin(); i != edges.end(); i++)
		d[i.from().id()][i.to().id()] = 1, d[i.to().id()][i.from().id()] = 1;

	for (unsigned i = 0; i < nodes.count(); i++)
		for (unsigned j = 0; j < nodes.count(); j++)
			for (unsigned k = 0; k < nodes.count(); k++)
				if (d[i][j] > d[i][k] + d[k][j])
					d[i][j] = d[i][k] + d[k][j];

	return d;
}

int cloudsim::PhysicalNetwork::diameter()
{
	if (_diameter != -1)
		return _diameter;

	int max = 0;
	std::vector<int>::iterator max_row;

	if (_distance_matrix.size() != nodes.count())
		_distance_matrix = distance_matrix();

	for (std::vector<int> row : _distance_matrix) {
		max_row = std::max_element(row.begin(), row.end());
		max = *max_row > max ? *max_row : max;
	}

	return _diameter = max;
}

cloudsim::PhysicalNetwork::~PhysicalNetwork() { }

void cloudsim::PhysicalNetwork::_read_from(std::string& file_name) throw (std::runtime_error)
{
	using json = nlohmann::json;
	std::ifstream fs(file_name);
	json j;

	if (fs.is_open())
		fs >> j, fs.close();
	else
		throw std::runtime_error("couldn't open file");

	for (auto& node : j["nodes"]) {
		std::vector<unsigned> capacity(node["capacity"].begin(), node["capacity"].end());
		nodes.add(new PhysicalNode(capacity, node["type"].get<unsigned>(),
								   node["group"].get<unsigned long>()));
	}

	for (auto& edge : j["edges"])
		edges.add(
			nodes[edge["from"].get<unsigned>()],
			nodes[edge["to"].get<unsigned>()],
			new PhysicalEdge(edge["capacity"].get<unsigned>(), edge["group"].get<unsigned long>())
		);
}
