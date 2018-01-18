
#include <iostream>
#include "rest_request_handler.h"

#include <json/json.h>
#include <regex>
using json = nlohmann::json;

RESTRequestHandler::RESTRequestHandler(NetworkManager* mgr)
	: Net::HTTPRequestHandler(), _network_manager(mgr) { }

void RESTRequestHandler
	::handleRequest(Net::HTTPServerRequest& request, Net::HTTPServerResponse& response)
{
	try {
		std::cout << "[RESTRequestHandler] " << request.getMethod() << " "
				  << request.getURI() << std::endl;

		if (request.getMethod() == "GET" && request.getURI() == "/physical_network.json") {
			_request_physical_network(request, response);
		} else if (request.getMethod() == "GET" && request.getURI() == "/utilization.json") {
			_request_utilization(request, response);
		} else if (request.getMethod() == "GET" && request.getURI() == "/vnets") {
			_request_virtual_networks(request, response);
		} else if (request.getMethod() == "POST" && request.getURI() == "/vnets") {
			_request_new_virtual_network(request, response);
		} else if (request.getMethod() == "DELETE" && std::regex_match(request.getURI(),
			std::regex("^/vnets/([0-9]+)$"))) {
			_request_delete_virtual_network(request, response);
		} else if (request.getMethod() == "PUT" && std::regex_match(request.getURI(),
			std::regex("^/vnets/([0-9]+)$"))) {
			_request_expand_virtual_network(request, response);
		} else if (request.getMethod() == "POST" && request.getURI() == "/vnets/remap") {
			_request_remap(request, response);
		} else {
			response.setStatus(Net::HTTPResponse::HTTP_NOT_FOUND);
			response.send();
		}
	} catch (std::exception& e) {
		std::cout << "caught exception: " << e.what() << std::endl;
	}
}

RESTRequestHandler::~RESTRequestHandler() { }

void RESTRequestHandler
	::_request_physical_network(Net::HTTPServerRequest& request, Net::HTTPServerResponse& response)
{
	cloudsim::PhysicalNetwork* net = _network_manager->physical_network();
	response.setStatus(Net::HTTPServerResponse::HTTP_OK);
	response.setContentType("text/json");
	std::ostream& out = response.send();

	json j;
	j["nodes"] = {}, j["links"] = {};

	for (auto i = net->nodes.begin(); i != net->nodes.end(); ++i)
		j["nodes"].push_back({
			{"id", i.id()},
			{"type", dynamic_cast<cloudsim::PhysicalNode*>(*i)->type() },
			{"capacity", dynamic_cast<cloudsim::PhysicalNode*>(*i)->capacity() },
			{"utilization", { -1, - 1}}
		});

	for (auto i = net->links.begin(); i != net->links.end(); ++i)
		j["links"].push_back({
			{"id", i.id()},
			{"from", i.from().id()},
			{"to", i.to().id()},
			{"capacity", dynamic_cast<cloudsim::PhysicalEdge*>(*i)->capacity() },
			{"utilization", -1}
		});

	out << j.dump() << std::flush;
}

void RESTRequestHandler
	::_request_utilization(Net::HTTPServerRequest& request, Net::HTTPServerResponse& response)
{
	cloudsim::PhysicalNetwork* net = _network_manager->physical_network();
	response.setStatus(Net::HTTPServerResponse::HTTP_OK);
	response.setContentType("text/json");
	std::ostream& out = response.send();

	json j;

	if (_network_manager->statistics().empty()) {
		j["server_util"] = 0, j["switch_util"] = 0, j["edge_util"] = 0;
		j["server_var"] = 0, j["switch_var"] = 0, j["edge_var"] = 0;
		j["virtual_networks"] = 0;
	} else {
		j["server_util"] = _network_manager->statistics().last().rel_server_utilization;
		j["switch_util"] = _network_manager->statistics().last().rel_switch_utilization;
		j["edge_util"]   = _network_manager->statistics().last().rel_link_utilization;
		j["server_var"]  = _network_manager->statistics().last().rel_server_utilization_variance;
		j["switch_var"]  = _network_manager->statistics().last().rel_switch_utilization_variance;
		j["edge_var"]    = _network_manager->statistics().last().rel_link_utilization_variance;
		j["virtual_networks"] = net->subgraphs().size();
	}

	j["nodes"] = {}, j["links"] = {};

	for (auto i = net->nodes.begin(); i != net->nodes.end(); ++i) {
		cloudsim::PhysicalNode* n = dynamic_cast<cloudsim::PhysicalNode*>(*i);
		j["nodes"].push_back({n->utilization(0) / n->capacity(0), n->utilization(1) / n->capacity(1)});
	}
//
	for (auto i = net->links.begin(); i != net->links.end(); ++i) {
		cloudsim::PhysicalEdge* e = dynamic_cast<cloudsim::PhysicalEdge*>(*i);
		j["links"].push_back(e->utilization() / e->capacity());
	}

	out << j.dump() << std::flush;
}

void RESTRequestHandler
	::_request_virtual_networks(Net::HTTPServerRequest& request, Net::HTTPServerResponse& response)
{
	response.setStatus(Net::HTTPServerResponse::HTTP_OK);
	response.setContentType("text/json");
	std::ostream& out = response.send();
	json j = {};
	j["networks"] = {};

	for (auto s : _network_manager->physical_network()->subgraphs()) {
		auto v = static_cast<cloudsim::VirtualNetwork*>(s);
		j["networks"].push_back({{"id", v->id()}, {"n", v->nodes.count()}, {"m", v->edges.count()}});
	}

	out << j.dump();
	out.flush();
}

void RESTRequestHandler
	::_request_new_virtual_network(Net::HTTPServerRequest& request, Net::HTTPServerResponse& response)
{
	std::string body = _body_from_request(request);
	response.setStatus(Net::HTTPServerResponse::HTTP_OK);
	response.setContentType("text/json");
	std::ostream& out = response.send();
	json j = {};

	cloudsim::VirtualNetwork* v = cloudsim::VirtualNetwork::from_json_string(body);
	j["success"] = _network_manager->new_virtual_network(v);
	out << j.dump();
	out.flush();
}

void RESTRequestHandler
	::_request_delete_virtual_network(Net::HTTPServerRequest& request, Net::HTTPServerResponse& response)
{
	std::match_results<std::string::const_iterator> matches;
	std::regex_match(request.getURI(), matches, std::regex("^/vnets/([0-9]+)$"));

	if (matches.size() != 2) {
		response.setStatus(Net::HTTPResponse::HTTP_NOT_FOUND);
		response.send();
	} else {
		unsigned long net_id = std::stoul(matches[1]);
		cloudsim::VirtualNetwork* v = nullptr;

		try {
			v = _network_manager->physical_network()->virtual_network(net_id);
		} catch (cloudsim::logic_error& e) {
			response.setStatus(Net::HTTPResponse::HTTP_NOT_FOUND);
			response.send();
			return;
		}

		_network_manager->delete_virtual_network(v);
		response.setStatus(Net::HTTPServerResponse::HTTP_NO_CONTENT);
		response.send();
	}
}

void RESTRequestHandler
	::_request_remap(Net::HTTPServerRequest& request, Net::HTTPServerResponse& response)
{
	_network_manager->remap();
	response.setStatus(Net::HTTPServerResponse::HTTP_NO_CONTENT);
	response.send();
}

void RESTRequestHandler
	::_request_expand_virtual_network(Net::HTTPServerRequest& request, Net::HTTPServerResponse& response)
{
	std::match_results<std::string::const_iterator> matches;
	std::regex_match(request.getURI(), matches, std::regex("^/vnets/([0-9]+)$"));

	if (matches.size() != 2) {
		response.setStatus(Net::HTTPResponse::HTTP_NOT_FOUND);
		response.send();
	} else {
		unsigned long net_id = std::stoul(matches[1]);
		cloudsim::VirtualNetwork* v = nullptr;

		try {
			v = _network_manager->physical_network()->virtual_network(net_id);
		} catch (cloudsim::logic_error& e) {
			response.setStatus(Net::HTTPResponse::HTTP_NOT_FOUND);
			response.send();
			return;
		}

		_network_manager->expand_virtual_network(v);

		response.setStatus(Net::HTTPServerResponse::HTTP_NO_CONTENT);
		response.send();
	}
}

std::string RESTRequestHandler::_body_from_request(Net::HTTPServerRequest& request)
{
	long len = request.getContentLength();
	char* buffer = new char[len];
	std::fill(buffer, buffer + len, '\0');
	request.stream().read(buffer, len);
	std::string str(buffer);
	delete[] buffer;
	str.resize(len);
	return str;
}
