#ifndef VDE_PHYSICAL_NETWORK_STATUS_H
#define VDE_PHYSICAL_NETWORK_STATUS_H

#include <vector>
#include <json/json.h>
#include <cloudsim/physical_network.h>

namespace util {
	class PhysicalNetworkStatus
	{
	public:

		using json = nlohmann::json;

		PhysicalNetworkStatus();

		unsigned long _recent_request;
		bool _success;
		double _duration;
		std::vector<double> _switch_util;
		std::vector<double> _server_util;
		std::vector<double> _edge_util;

		void fill(unsigned long recent_request, bool success, double duration, cloudsim::PhysicalNetwork& p);

		json to_json();


	};
}
#endif
