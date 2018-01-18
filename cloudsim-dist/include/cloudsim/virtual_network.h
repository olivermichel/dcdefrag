
#ifndef CLOUDSIM_VIRTUAL_NETWORK_H
#define CLOUDSIM_VIRTUAL_NETWORK_H

#include <plexum/graph.h>
#include <stdexcept>
#include <string>
#include <functional>
#include <vector>
#include <json/json.h>

#include "node.h"
#include "edge.h"
#include "virtual_node.h"
#include "virtual_edge.h"
#include "mapping.h"

namespace cloudsim {

	using json = nlohmann::json;

	class Mapping;

	class VirtualNode;
	class VirtualEdge;

	class VirtualNetwork : public plexum::Graph<Node*, Edge*>
	{
	public:

		VirtualNetwork(unsigned long id, unsigned long type);
		VirtualNetwork(std::string file_name) throw (std::runtime_error);
		VirtualNetwork(VirtualNetwork& copy_from);

		bool all_nodes(std::function<bool (VirtualNode*)> f);
		bool all_edges(std::function<bool (VirtualEdge*)> f);

		void untouch_all_nodes();

		unsigned long id() const;
		unsigned long type() const;

		double total_node_demand(unsigned long node_type, unsigned long resource_type);
		double total_edge_demand(unsigned long resource_type);

		void add_mapping(cloudsim::Mapping m);
		std::vector<cloudsim::Mapping> mappings() const;
		cloudsim::Mapping& last_mapping() throw(std::logic_error);

		vertex_proxy& nodes;
		edge_proxy& links;

		static VirtualNetwork* from_json_file(std::string file_name) throw (std::runtime_error);
		static VirtualNetwork* from_json_string(std::string json) throw (std::runtime_error);

		json _to_json();
		void _from_json(json& j);

		virtual ~VirtualNetwork();

		VirtualNetwork(); // private constructor is used by static generators (do not set = delete)
	private:
		unsigned long _id;
		unsigned long _type;
		std::vector<cloudsim::Mapping> _mappings;
		void _read_json_file(std::string file_name) throw (std::runtime_error);
		void _read_json_string(std::string json) throw (std::runtime_error);
	};
}

#endif
