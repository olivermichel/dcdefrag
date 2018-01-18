
#ifndef VDE_REST_REQUEST_HANDLER_H
#define VDE_REST_REQUEST_HANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "network_manager.h"

using namespace Poco;


class RESTRequestHandler : public Net::HTTPRequestHandler
{
public:
	RESTRequestHandler(NetworkManager*);
	virtual void handleRequest(Net::HTTPServerRequest&, Net::HTTPServerResponse&);

	~RESTRequestHandler();
private:
	void _request_physical_network(Net::HTTPServerRequest&, Net::HTTPServerResponse&);
	void _request_utilization(Net::HTTPServerRequest&, Net::HTTPServerResponse&);
	void _request_virtual_networks(Net::HTTPServerRequest&, Net::HTTPServerResponse&);
	void _request_new_virtual_network(Net::HTTPServerRequest&, Net::HTTPServerResponse&);
	void _request_delete_virtual_network(Net::HTTPServerRequest&, Net::HTTPServerResponse&);
	void _request_remap(Net::HTTPServerRequest&, Net::HTTPServerResponse&);
	void _request_expand_virtual_network(Net::HTTPServerRequest&, Net::HTTPServerResponse&);

	std::string _body_from_request(Net::HTTPServerRequest& request);
	NetworkManager* _network_manager;
};


#endif
