#ifndef VDE_CONTEXT_H
#define VDE_CONTEXT_H

#include <functional>
#include <cloudsim/physical_network.h>
#include "../algo/report.h"

#include <json/json.h>
#include <random>

using json = nlohmann::json;

class NetworkManager
{
public:

	enum class op_type {
		map          = 0,
		unmap        = 1,
		expand       = 2,
		contract     = 3
	};

	struct data_point {
		data_point() = delete;
		data_point(op_type type, unsigned long net_id, bool accept, double time,
				   cloudsim::PhysicalNetwork* p);

		op_type       type;
		unsigned long net_id;
		bool          accept;
		double        time;

		double rel_server_utilization;
		double rel_switch_utilization;
		double rel_link_utilization;

		double rel_server_utilization_variance;
		double rel_switch_utilization_variance;
		double rel_link_utilization_variance;

		unsigned long vn_count;

		std::vector<std::vector<double>> path_length_distribution;

		std::vector<double> rel_server_utilization_distribution;
		std::vector<double> rel_switch_utilization_distribution;
		std::vector<double> rel_link_utilization_distribution;

		json to_json() const;
	};

	struct statistics {
		statistics() = default;
		std::list<data_point> data;

		bool empty() const;
		const data_point& last() throw(std::logic_error);
		json to_json() const;
		void write_to_file(std::string file_name);
	};

	NetworkManager() = delete;
	explicit NetworkManager(cloudsim::PhysicalNetwork*);
	NetworkManager(cloudsim::PhysicalNetwork*, std::string out_path);
	cloudsim::PhysicalNetwork* physical_network() const;

	bool map_virtual_network(std::unique_ptr<cloudsim::VirtualNetwork> v);
	bool expand_virtual_network(std::unique_ptr<cloudsim::VirtualNetwork>& v, unsigned long a);
	void contract_virtual_network(std::unique_ptr<cloudsim::VirtualNetwork>& v, unsigned long a);
	void unmap_virtual_network(std::unique_ptr<cloudsim::VirtualNetwork>& v);


	bool virtual_network_exists(unsigned long r);
	std::unique_ptr<cloudsim::VirtualNetwork>& random_virtual_network(std::random_device& r) throw(std::logic_error);
	std::unique_ptr<cloudsim::VirtualNetwork>& virtual_network_by_id(unsigned long r) throw(std::logic_error);

	std::map<unsigned long, bool> remap();

	void save_configuration(std::string file_name);
	void load_configuration(std::string file_name);

	statistics& statistics();

private:

	cloudsim::PhysicalNetwork* _physical_network;
	std::vector<std::vector<int>> _distance_matrix;
	std::vector<std::unique_ptr<cloudsim::VirtualNetwork>> _networks;

	unsigned long _counter;
	struct statistics _stats;
	bool _request_running;
};

#endif
