#include "server.h"
#include "request_handler_factory.h"
#include "network_manager.h"

#include <iostream>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

int Server::main(const std::vector<std::string>& argv)
{
	if (argv.size() != 2) {
		std::cerr << "usage: vdce_server <substrate.json> <out.dir>" << std::endl;
		return 1;
	}

	std::cout << "[Server] Loading Physical Network from " << argv[0] << std::endl;

	NetworkManager simulator_context(new cloudsim::PhysicalNetwork(argv[0]), std::string(argv[1]));
	std::cout << "[Server] writing output to " + std::string(argv[1]) << std::endl;

	Poco::Net::ServerSocket socket(9090);
	Poco::Net::HTTPServerParams* server_params = new Poco::Net::HTTPServerParams();
	server_params->setMaxQueued(100);
	server_params->setMaxThreads(16);

	RequestHandlerFactory* handlerFactory = new RequestHandlerFactory(&simulator_context);
	Net::HTTPServer server(handlerFactory, socket, server_params);

	server.start();
	std::cout << "[Server] API available at http://127.0.0.1:9090" << std::endl;
	this->waitForTerminationRequest();
	server.stop();
	return 0;
}
