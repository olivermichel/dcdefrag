#include "request_handler_factory.h"
#include "rest_request_handler.h"

RequestHandlerFactory::RequestHandlerFactory(NetworkManager* context)
	: Net::HTTPRequestHandlerFactory(), _context(context) { }

Net::HTTPRequestHandler*
RequestHandlerFactory::createRequestHandler(const Net::HTTPServerRequest& request)
{
	return new RESTRequestHandler(_context);
}
