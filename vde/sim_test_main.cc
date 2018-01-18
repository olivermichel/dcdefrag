#include "server/network_manager.h"
#include "util/sim_helpers.h"

#include <fstream>

#include <json/json.h>
#include <args/args.h>

using json = nlohmann::json;

class SimulatorTest
{
	struct opt_t
	{
		args::ArgumentParser           parser;
		args::Positional<std::string>  pnet_file;
		args::Flag                     debug;
		args::HelpFlag                 help;

		opt_t()
			: parser("sim_test", "dcdefrag test simulator"),
			  pnet_file(parser, "pnet.json", "path to physical network file, required"),
			  debug(parser, "debug", "enable debug mode", {'d', "debug"}),
			  help(parser, "help", "display this help menu", {'h', "help"}) { }
	};

public:

	int operator()(int argc, const char** argv, std::ostream& out = std::cout,
				   std::ostream& err = std::cerr)
	{
		opt_t opt;
		_parse_options(opt, argc, argv, out, err);

		cloudsim::PhysicalNetwork p(args::get(opt.pnet_file));
		NetworkManager manager(&p, "sim_test");

		std::list<cloudsim::VirtualNetwork*> networks;
		std::list<cloudsim::VirtualNetwork*>::iterator v;
		cloudsim::VirtualNetwork* n = nullptr;

		unsigned long i = 0;

		n = sim::helpers::vc(i++, 2, 50, 50);
		manager.new_virtual_network(n);
		networks.insert(std::end(networks), n);

		////

		n = sim::helpers::vc(i++, 2, 50, 50);
		manager.new_virtual_network(n);
		networks.insert(std::end(networks), n);

		manager.save_configuration("test_config-0.json");

		////

		v = sim::helpers::network_by_id(networks, 0);
		i++;

		if (manager.expand_virtual_network(*v, 3))
			out << "[Simulator][" << std::setw(3) << i << "] expand successful" << std::endl;
		else
			out << "[Simulator][" << std::setw(3) << i << "] expand failed" << std::endl;

		manager.save_configuration("test_config-1.json");

		////

		v = sim::helpers::network_by_id(networks, 0);
		i++;

		if (manager.expand_virtual_network(*v, 1))
			out << "[Simulator][" << std::setw(3) << i << "] expand successful" << std::endl;
		else
			out << "[Simulator][" << std::setw(3) << i << "] expand failed" << std::endl;

		////

		manager.save_configuration("test_config-2.json");

		return 0;
	}

private:

	void _parse_options(opt_t& opt, int argc, const char** argv, std::ostream& out = std::cout,
						std::ostream& err = std::cerr)
	{
		try {
			opt.parser.ParseCLI(argc, argv);
		} catch (args::Help&) {
			std::cout << opt.parser;
			exit(0);
		} catch (args::ParseError& e) {
			err << e.what() << std::endl;
			err << opt.parser;
			exit(1);
		}

		if (!opt.pnet_file) {
			err << opt.parser;
			exit(1);
		}
	}
};

int main(int argc, const char** argv)
{
	return SimulatorTest{}(argc, argv);
}
