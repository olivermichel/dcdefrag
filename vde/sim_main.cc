
#include <args/args.h>
#include <cloudsim/physical_network.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <json/json.h>
#include <list>
#include <random>
#include <set>

#include "server/network_manager.h"
#include "util/sim_helpers.h"

using json = nlohmann::json;

class Simulator
{
	struct opt_t
	{
		args::ArgumentParser               parser;
		args::HelpFlag                     help;
		args::Positional<std::string>      pnet_file;
		args::Positional<std::string>      wl_file;
		args::Positional<std::string>      exp_id;
		args::ValueFlag<std::string>       load_snapshot;
		args::ValueFlag<unsigned long>     load_save_iter;
		args::ValueFlagList<unsigned long> save;
		args::ValueFlagList<unsigned long> defrag;

		opt_t()
			: parser("sim", "dcdefrag simulator"),
			  help(parser, "help", "display this help menu", {'h', "help"}),
			  pnet_file(parser, "pnet.json", "path to physical network file, required"),
			  wl_file(parser, "wl.json", "path to workload file, required"),
			  exp_id(parser, "exp-id", "experiment identifier, required"),
			  load_snapshot(parser, "file", "load snapshot from [file]", {'l', "load"}),
			  load_save_iter(parser, "n",
							 "load/save snapshot at iteration [i], required if -l or -s is set",
							 {'i', "iter"}),
			  save(parser, "s", "save snapshot at iteration [i] (can be repeated", {'s', "save"}),
			  defrag(parser, "i", "defrag at iteration [i] (can be repeated)", {'d', "defrag"}) { }
	};

public:

	int operator()(int argc, const char** argv, std::ostream& out = std::cout,
				   std::ostream& err = std::cerr)
	{
		opt_t opt;
		_parse_options(opt, argc, argv, out, err);

		std::set<unsigned long> defrag_i, save_i;

		if (opt.defrag)
			for (const auto d: args::get(opt.defrag))
				defrag_i.insert(std::end(defrag_i), d);

		if (opt.save)
			for (const auto s: args::get(opt.save))
				save_i.insert(std::end(save_i), s);

		cloudsim::PhysicalNetwork p(args::get(opt.pnet_file));
		NetworkManager manager(&p, args::get(opt.exp_id));
		std::unique_ptr<cloudsim::VirtualNetwork> new_network;
		std::random_device rd;

		std::fstream fs(args::get(opt.wl_file));
		json wl_json;
		fs >> wl_json;
		json::iterator wl_it = wl_json.begin();

		if (opt.load_snapshot) {
			out << "[Simulator][   ] load snapshot from " << args::get(opt.load_snapshot)
				<< std::endl;

			std::advance(wl_it, args::get(opt.load_save_iter));
			manager.load_configuration(args::get(opt.load_snapshot));

			out << "[Simulator][   ] start at i=" << args::get(opt.load_save_iter) << std::endl;
			_print_utilization(out, p);

			manager.statistics().write_to_file(args::get(opt.exp_id) + "-report.json");
			out << "[Simulator][   ] written report "
					  << args::get(opt.exp_id) + "-report.json" << std::endl;
		}

		for (; wl_it != wl_json.end(); ++wl_it) {

			json step = *wl_it;
			unsigned long i = step["i"];

			// save before defrag if at same iteratior

			if (opt.save && save_i.find(i) != std::end(save_i)) {
				std::string file_name = args::get(opt.exp_id) + "-" + std::to_string(i) + ".json";
				manager.save_configuration(file_name);
				out << "[Simulator][" << std::setw(3) << i << "] written configuration to "
					<< file_name << std::endl;
			}

			if (opt.defrag && defrag_i.find(i) != std::end(defrag_i)) {
				out << "[Simulator]["<< std::setw(3) << i << "] remap " << std::endl;
				std::map<unsigned long, bool> remap_result = manager.remap();

				unsigned fail_counter = 0;

				for (auto k : remap_result) {
					if (!k.second) {
						fail_counter++;
					}
				}

				out << "[Simulator][   ] remap complete: " << fail_counter << " failed"
					<< std::endl;

				_print_utilization(std::cout, p);
			}

			enum sim::helpers::request_type r = sim::helpers::request_type(step["r"]);

			if (r == sim::helpers::request_type::new_network) {

				enum sim::helpers::network_type t = sim::helpers::network_type(step["t"]);

				if (t == sim::helpers::network_type::vc)
					new_network = sim::helpers::vc(i, step["n"], step["c"], step["b"]);
				else if (t == sim::helpers::network_type::voc)
					new_network = sim::helpers::voc(i, step["s"], step["m"], step["c"], step["b"],
													step["o"]);
				else if (t == sim::helpers::network_type::t3)
					new_network = sim::helpers::t3(i, step["alpha"], step["beta"], step["gamma"],
												   step["c"], step["b"]);

				manager.map_virtual_network(std::move(new_network));
				_print_utilization(out, p);

			} else {

				auto& v = manager.virtual_network_exists(step["j"]) ?
						  manager.virtual_network_by_id(step["j"])  :
						  manager.random_virtual_network(rd);

				if (r == sim::helpers::request_type::expand_network) {
					if (manager.expand_virtual_network(v, 20)) {
						out << "[Simulator][" << std::setw(3) << i << "] expand network successful"
							<< std::endl;
					} else {
						out << "[Simulator][" << std::setw(3) << i << "] expand network failed"
							<< std::endl;
					}
				} else if (r == sim::helpers::request_type::contract_network) {
					manager.contract_virtual_network(v, 20);
					out << "[Simulator][" << std::setw(3) << i << "] contract network successful"
						<< std::endl;
				} else if (r == sim::helpers::request_type::delete_network) {
					manager.unmap_virtual_network(v);

					out << "[Simulator][" << std::setw(3) << i << "] delete network successful"
						<< std::endl;
				}

				_print_utilization(out, p);
			}
		}

		manager.statistics().write_to_file(args::get(opt.exp_id) + "-report.json");
		out << "[Simulator][   ] written report " << args::get(opt.exp_id) + "-report.json"
			<< std::endl;

		return 0;
	}

private:

	void _parse_options(opt_t& opt, int argc, const char** argv, std::ostream& out = std::cout,
					   std::ostream& err = std::cerr)
	{
		try {
			opt.parser.ParseCLI(argc, argv);
		} catch (args::Help&) {
			out << opt.parser;
			exit(1);
		} catch (args::ParseError& e) {
			err << e.what() << std::endl;
			err << opt.parser;
			exit(1);
		} catch (args::ValidationError& e) {
			err << e.what() << std::endl;
			err << opt.parser;
			exit(1);
		}

		// pnet_file, wl_file and exp_id must be set
		if (!opt.pnet_file || !opt.wl_file || !opt.exp_id) {
			err << opt.parser;
			exit(1);
		}

		// if -s or -l is specified, -i must be given
		if (opt.load_snapshot && !opt.load_save_iter) {
			err << opt.parser;
			exit(1);
		}
	}

	void _print_utilization(std::ostream& os, cloudsim::PhysicalNetwork& net)
	{
		os  << "[Simulator][   ] utilization: x=" << std::setprecision(3)
			<< net.rel_node_utilization(0, 0) << " / s=" << std::setprecision(3)
			<< net.rel_node_utilization(0, 1) << " / e=" << std::setprecision(3)
			<< net.rel_edge_utilization(0) << std::endl;
	}
};

int main(int argc, const char** argv)
{
	return Simulator{}(argc, argv);
}
