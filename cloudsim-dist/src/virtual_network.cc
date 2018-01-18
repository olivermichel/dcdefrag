
#include "cloudsim/virtual_network.h"
#include "cloudsim/node.h"

#include <fstream>
#include <json/json.h>

using json = nlohmann::json;

cloudsim::VirtualNetwork::VirtualNetwork(unsigned long id, unsigned long type)
	: plexum::Graph<Node*, Edge*>(),
	  nodes(vertices),
	  links(edges),
	  _id(id),
	  _type(type),
	  _mappings()
{ }

cloudsim::VirtualNetwork::VirtualNetwork(std::string file_name) throw (std::runtime_error)
	: plexum::Graph<Node*, Edge*>(),
	  nodes(vertices),
	  links(edges),
	  _mappings()
{
	_read_json_file(file_name);
}

cloudsim::VirtualNetwork::VirtualNetwork(VirtualNetwork& copy_from)
	: plexum::Graph<Node*, Edge*>(copy_from),
	  nodes(copy_from.vertices),
	  links(copy_from.edges),
	  _id(copy_from._id),
	  _type(copy_from._type),
	  _mappings(copy_from._mappings)
{ }

bool cloudsim::VirtualNetwork::all_nodes(std::function<bool (cloudsim::VirtualNode*)> f)
{
	for (auto n : nodes)
		if (!f(dynamic_cast<VirtualNode*>(n))) return false;

	return true;
}


bool cloudsim::VirtualNetwork::all_edges(std::function<bool (cloudsim::VirtualEdge*)> f)
{
	for (auto e : edges)
		if (!f(dynamic_cast<VirtualEdge*>(e))) return false;

	return true;
}

void cloudsim::VirtualNetwork::untouch_all_nodes()
{
	for (auto n : nodes) {
		VirtualNode* vnode = dynamic_cast<VirtualNode*>(n);
		if (vnode->touched())
			vnode->untouch();
	}
}

unsigned long cloudsim::VirtualNetwork::id() const
{
	return _id;
}

unsigned long cloudsim::VirtualNetwork::type() const
{
	return _type;
}

double cloudsim::VirtualNetwork::total_node_demand(unsigned long node_type, unsigned long resource_type)
{
	return std::accumulate(vertices.begin(), vertices.end(), 0.0,
		[&node_type, &resource_type] (double a, cloudsim::Node* abstract_node) {
		cloudsim::VirtualNode* node = dynamic_cast<cloudsim::VirtualNode*>(abstract_node);
		return node->type() == node_type ? a + node->demand(resource_type) : a;
	});
}

double cloudsim::VirtualNetwork::total_edge_demand(unsigned long resource_type)
{
	return std::accumulate(edges.begin(), edges.end(), 0.0,
		[&resource_type] (double a, cloudsim::Edge* abstract_edge) {
		return a + dynamic_cast<cloudsim::VirtualEdge*>(abstract_edge)->demand(resource_type);
	});
}

void cloudsim::VirtualNetwork::add_mapping(cloudsim::Mapping m)
{
	_mappings.push_back(m);
}

std::vector<cloudsim::Mapping> cloudsim::VirtualNetwork::mappings() const
{
	return _mappings;
}

cloudsim::Mapping& cloudsim::VirtualNetwork::last_mapping()
	throw(std::logic_error)
{
	if (_mappings.empty())
		throw std::logic_error("VirtualNetwork::last_mapping(): network has not been mapped");

	return _mappings.back();
}

cloudsim::VirtualNetwork* cloudsim::VirtualNetwork::from_json_file(std::string file_name)
	throw (std::runtime_error)
{
	cloudsim::VirtualNetwork* v = new cloudsim::VirtualNetwork();
	v->_read_json_file(file_name);
	return v;
}

cloudsim::VirtualNetwork* cloudsim::VirtualNetwork::from_json_string(std::string json)
	throw (std::runtime_error)
{
	cloudsim::VirtualNetwork* v = new cloudsim::VirtualNetwork();
	v->_read_json_string(json);
	return v;
}

json cloudsim::VirtualNetwork::_to_json()
{
	json j {{"id", _id}, {"type", _type}, {"nodes", json::array() }, {"edges", json::array() }};

	for (auto node_i = std::begin(nodes); node_i != std::end(nodes); node_i++) {
		VirtualNode* node = dynamic_cast<VirtualNode*>(*node_i);
		json node_obj {
			{"id",  node->id()}, {"type", node->type()}, {"group", node->group()},
			{"demand", {node->demand(0), node->demand(1)}}
		};

		j["nodes"].push_back(node_obj);
	}

	for (auto edge_i = std::begin(edges); edge_i != std::end(edges); edge_i++) {
		VirtualEdge* edge = dynamic_cast<VirtualEdge*>(*edge_i);

		unsigned long from_id = dynamic_cast<VirtualNode*>(*(edge_i.from()))->id();
		unsigned long to_id   = dynamic_cast<VirtualNode*>(*(edge_i.to()))->id();

		json edge_obj {
			{"id",  edge->id() }, {"from", from_id }, {"to", to_id}, {"demand", edge->demand(0)}
		};

		j["edges"].push_back(edge_obj);
	}

	return j;
}

void cloudsim::VirtualNetwork::_from_json(nlohmann::json& j)
{
	_id = j["id"].get<unsigned long>();
	_type = j["type"].get<unsigned long>();

	for (auto& node : j["nodes"]) {
		std::vector<unsigned> d(node["demand"].begin(), node["demand"].end());
		nodes.add(new VirtualNode(node["id"].get<unsigned long>(), d, node["type"].get<unsigned long>(), node["group"].get<unsigned long>(), this));
	}

	for (auto& edge : j["edges"]) {
		edges.add(
			nodes[edge["from"].get<unsigned>()],
			nodes[edge["to"].get<unsigned>()],
			new VirtualEdge(edge["id"].get<unsigned long>(), edge["demand"].get<double>(), 0, this)
		);
	}
}

cloudsim::VirtualNetwork::~VirtualNetwork() { }

cloudsim::VirtualNetwork::VirtualNetwork()
	: plexum::Graph<Node*, Edge*>(),
	  nodes(vertices),
	  links(edges) { }

void cloudsim::VirtualNetwork::_read_json_string(std::string str) throw (std::runtime_error)
{
	using json = nlohmann::json;
	json j = json::parse(str);

	_id = j["id"].get<unsigned long>();
	_type = j["type"].get<unsigned long>();

	for (auto& node : j["nodes"]) {
		std::vector<unsigned> d(node["demand"].begin(), node["demand"].end());
		nodes.add(new VirtualNode(node["id"].get<unsigned long>(), d, node["type"].get<unsigned long>(), node["group"].get<unsigned long>(), this));
	}

	for (auto& edge : j["edges"])
		edges.add(
			nodes[edge["from"].get<unsigned>()],
			nodes[edge["to"].get<unsigned>()],
			new VirtualEdge(edge["id"].get<unsigned long>(), edge["demand"].get<double>(), 0, this)
		);
}

void cloudsim::VirtualNetwork::_read_json_file(std::string file_name) throw (std::runtime_error)
{
	using json = nlohmann::json;
	std::ifstream fs(file_name);
	json j;

	if (fs.is_open())
		fs >> j, fs.close();
	else
		throw std::runtime_error("couldn't open file");

	_id = j["id"].get<unsigned long>();
	_type = j["type"].get<unsigned long>();

	for (auto& node : j["nodes"]) {
		std::vector<unsigned> d(node["demand"].begin(), node["demand"].end());
		nodes.add(new VirtualNode(node["id"].get<unsigned long>(), d, node["type"].get<unsigned long>(), node["group"].get<unsigned long>(), this));
	}

	for (auto& edge : j["edges"])
		edges.add(
			nodes[edge["from"].get<unsigned>()],
			nodes[edge["to"].get<unsigned>()],
			new VirtualEdge(edge["id"].get<unsigned long>(), edge["demand"].get<double>(), 0, this)
		);
}
