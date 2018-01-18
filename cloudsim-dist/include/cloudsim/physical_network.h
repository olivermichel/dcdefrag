
#ifndef CLOUDSIM_PHYSICAL_NETWORK_H
#define CLOUDSIM_PHYSICAL_NETWORK_H

#include <plexum/graph.h>
#include <stdexcept>
#include <string>
#include <functional>

#include "virtual_network.h"
#include "mapping.h"
#include "node.h"
#include "edge.h"
#include "logic_error.h"

#include "physical_node.h"
#include "physical_edge.h"

namespace cloudsim {

	class VirtualNetwork;

	class Mapping;

	class PhysicalNetwork : public plexum::Graph<Node*, Edge*>
	{
	public:
		PhysicalNetwork();
		PhysicalNetwork(std::string file_name) throw (std::runtime_error);
		PhysicalNetwork(PhysicalNetwork& copy_from);

		void reset_node_reservations();

		void block_node_reservations();

		void reset_edge_reservations();

		void block_edge_reservations();
/*
		template<typename T>
		std::vector<T> node_distribution(std::function<T (cloudsim::PhysicalNode*)> f);

		template<typename T>
		std::vector<T>
		node_distribution(std::function<T (cloudsim::PhysicalNode*)> f, unsigned long type = 0);

		template<typename T>
		std::vector<T> edge_distribution(std::function<T (cloudsim::PhysicalEdge*)> f);
*/

		unsigned long count_nodes(unsigned long node_type);

		unsigned total_node_capacity(unsigned long resource_type);

		unsigned total_node_capacity(unsigned long resource_type, unsigned long node_type);

		unsigned total_edge_capacity(unsigned long resource_type);

		unsigned total_node_utilization(unsigned long resource_type);

		unsigned total_node_utilization(unsigned long resource_type, unsigned long node_type);

		unsigned total_edge_utilization(unsigned long resource_type);

		double rel_node_utilization(unsigned long resource_type);

		double rel_node_utilization(unsigned long resource_type, unsigned long node_type);

		double rel_edge_utilization(unsigned long resource_type);

		double avg_node_utilization(unsigned long resource_type);

		double avg_node_utilization(unsigned long resource_type, unsigned long node_type);

		double avg_edge_utilization(unsigned long resource_type);

		unsigned total_node_residual(unsigned long resource_type);

		unsigned total_node_residual(unsigned long resource_type, unsigned long node_type);

		unsigned total_edge_residual(unsigned long resource_type);

		double avg_node_residual(unsigned long resource_type);

		double avg_node_residual(unsigned long resource_type, unsigned long node_type);

		double avg_edge_residual(unsigned long resource_type);

		double node_utilization_variance(unsigned long resource_type);

		double node_utilization_variance(unsigned long resource_type, unsigned long node_type);

		double edge_utilization_variance(unsigned long resource_type);

		double rel_node_utilization_variance(unsigned long resource_type);

		double rel_node_utilization_variance(unsigned long resource_type, unsigned long node_type);

		double rel_edge_utilization_variance(unsigned long resource_type);

		std::vector<unsigned> node_utilization_distribution(unsigned long resource_type);

		std::vector<unsigned> node_utilization_distribution(unsigned long resource_type,
			unsigned long node_type);

		std::vector<unsigned> edge_utilization_distribution(unsigned long resource_type);
/*
		std::vector<double> node_residual_distribution(unsigned long resource_type);

		std::vector<double> node_residual_distribution(unsigned long resource_type,
			unsigned long node_type);

		std::vector<double> edge_residual_distribution(unsigned long resource_type);
*/
		std::vector<double> rel_node_utilization_distribution(unsigned long resource_type);

		std::vector<double> rel_node_utilization_distribution(unsigned long resource_type,
			unsigned long node_type);

		std::vector<double> rel_edge_utilization_distribution(unsigned long resource_type);

		void add_virtual_network(VirtualNetwork* v, cloudsim::Mapping& m);

		void remove_virtual_network(VirtualNetwork* v);

		VirtualNetwork* virtual_network(unsigned long id) throw (logic_error);

		VirtualNetwork* random_virtual_network() throw (logic_error);

		std::vector<VirtualNetwork*> virtual_networks();

		std::vector<double> avg_path_length_distribution();

		double avg_path_length();

		std::vector<std::vector<int>> distance_matrix();

		int diameter();

		virtual ~PhysicalNetwork();

		vertex_proxy& nodes;
		edge_proxy& links;

		std::vector<std::vector<int>> _distance_matrix;

		int _diameter;

	private:
		void _read_from(std::string& file_name) throw (std::runtime_error);
	};
}

#endif
