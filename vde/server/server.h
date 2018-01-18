#ifndef VDE_VDCESERVER_H
#define VDE_VDCESERVER_H

#include <vector>
#include <string>

#include <Poco/Util/ServerApplication.h>

using namespace Poco;

class Server : public Util::ServerApplication
{
protected:
	int main(const std::vector<std::string>& argv);
};

#endif
