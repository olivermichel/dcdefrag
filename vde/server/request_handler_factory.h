#ifndef VDE_REQUEST_HANDLER_FACTORY_H
#define VDE_REQUEST_HANDLER_FACTORY_H

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include "network_manager.h"

using namespace Poco;

class NetworkManager;

class RequestHandlerFactory : public Net::HTTPRequestHandlerFactory
{
public:
	RequestHandlerFactory(NetworkManager* context);
	virtual Net::HTTPRequestHandler* createRequestHandler(const Net::HTTPServerRequest& r);

private:
	NetworkManager* _context;
};

#endif
