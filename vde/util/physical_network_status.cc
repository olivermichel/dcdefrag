
#include "physical_network_status.h"


util::PhysicalNetworkStatus::PhysicalNetworkStatus()
	: _recent_request(0), _success(false), _duration(0.0), _switch_util(), _server_util(), _edge_util()
{ }

void util::PhysicalNetworkStatus::fill(unsigned long recent_request, bool success, double duration,
									   cloudsim::PhysicalNetwork& p)
{
	_recent_request = recent_request;
	_success = success;
	_duration = duration;
	_switch_util = p.rel_node_utilization_distribution(0);
	_server_util = p.rel_node_utilization_distribution(1);
	_edge_util = p.rel_edge_utilization_distribution(0);
}


nlohmann::json util::PhysicalNetworkStatus::to_json()
{
	nlohmann::json j;

	j["recent_request"] = _recent_request;
	j["success"]        = _success;
	j["duration"]       = _duration;
	j["switch_util"]    = _switch_util;
	j["server_util"]    = _server_util;
	j["edge_util"]      = _edge_util;

	return j;
}
