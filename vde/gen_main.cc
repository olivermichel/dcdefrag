
#include "util/sim_helpers.h"

#include <fstream>
#include <args/args.h>

using json = nlohmann::json;

template<typename T>
typename T::iterator random_element(T& list, std::random_device& r)
{
	auto i = std::begin(list);
	std::advance(i, sim::helpers::_u<unsigned long>(0, list.size() - 1, r));
	return i;
}

class Generator
{

	struct opt_t {
		args::ArgumentParser            parser;
		args::HelpFlag                  help;
		args::ValueFlag<unsigned long>  a;
		args::Positional<unsigned long> n;
		args::Positional<std::string>   out_file;

		opt_t()
			: parser("gen","workload generator for dcdefrag simulator"),
			  help(parser, "help", "display this help menu", {'h', "help"}),
			  a(parser, "a", "number of network additions at beginning", {'a',"additions"}),
			  n(parser, "n", "number of iterations"),
			  out_file(parser, "out.json", "output file") { }
	};

	struct network {

		network(unsigned long i, unsigned long n) : i(i), n(n) { }

		network& operator+=(unsigned long m) {
			n += m;
			return *this;
		}

		network& operator-=(unsigned long m) {
			n -= m;
			return *this;
		}

		unsigned long i, n;
	};

public:

	int operator()(int argc, const char** argv) {
		opt_t opt;
		parse_options(opt, argc, argv);

		std::list<network> networks;
		std::random_device random_device;
		std::random_device r;
		std::ofstream of(args::get(opt.out_file));

		// new, expand, contract, delete
		std::discrete_distribution<> request_type_distribution{ 0.25, 0.5, 0, 0.25 };

		json out = json::array();

		unsigned long j = 0, a = opt.a ? args::get(opt.a) : 50;

		for(; j < a; j++)
			out.push_back(new_network(j, networks, r));

		for (unsigned long i = j; i < args::get(opt.n); i++) {

			auto request_type = (enum sim::helpers::request_type) request_type_distribution(r);

			if(request_type == sim::helpers::request_type::new_network) {
				out.push_back(new_network(i, networks, r));
			} else if (request_type == sim::helpers::request_type::expand_network) {
				out.push_back(expand_network(i, networks, r));
			} else if (request_type == sim::helpers::request_type::contract_network) {
				out.push_back(contract_network(i, networks, r));
			} else if (request_type == sim::helpers::request_type::delete_network) {
				out.push_back(delete_network(i, networks, r));
			}
		}

		of << out;
		of.close();

		return 0;
	}

private:

	void parse_options(opt_t& opt, int argc, const char** argv, std::ostream& err = std::cerr)
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

		if (!opt.n || !opt.out_file) {
			err << opt.parser;
			exit(1);
		}
	}

	json new_network(unsigned long i, std::list<network>& networks, std::random_device& r)
	{
		// VC:
		std::pair<unsigned, unsigned> vc_n = std::make_pair(10, 55);
		std::pair<unsigned, unsigned> vc_c = std::make_pair(50, 150);
		std::pair<unsigned, unsigned> vc_b = std::make_pair(50, 150);

		// VOC: n = s * m
		std::pair<unsigned, unsigned> voc_s = std::make_pair(2, 8);
		std::pair<unsigned, unsigned> voc_m = std::make_pair(2, 8);
		std::pair<unsigned, unsigned> voc_o = std::make_pair(1, 4);
		std::pair<unsigned, unsigned> voc_c = std::make_pair(50, 150);
		std::pair<unsigned, unsigned> voc_b = std::make_pair(50, 150);

		// T3
		std::pair<unsigned, unsigned> t3_alpha = std::make_pair(5, 20);
		std::pair<unsigned, unsigned> t3_beta = std::make_pair(5, 20);
		std::pair<unsigned, unsigned> t3_gamma = std::make_pair(5, 20);
		std::pair<unsigned, unsigned> t3_c = std::make_pair(50, 150);
		std::pair<unsigned, unsigned> t3_b = std::make_pair(50, 150);

		// VC, VOC, 3T
		std::discrete_distribution<> network_type_dist{1, 1, 1};

		json j = {{"i", i}, {"r", "new"}};
		auto network_type = (enum sim::helpers::network_type) network_type_dist(r);

		if (network_type == sim::helpers::network_type::vc) {

			auto n = sim::helpers::_u<unsigned>(vc_n.first, vc_n.second, r);
			j["t"]     = "vc";
			j["n"]     = n;
			j["c"]     = sim::helpers::_u<unsigned>(vc_c.first, vc_c.second, r);
			j["b"]     = sim::helpers::_u<unsigned>(vc_b.first, vc_b.second, r);
			networks.emplace_back(i,n);

		} else if (network_type == sim::helpers::network_type::voc) {

			auto s = sim::helpers::_u<unsigned long>(voc_s.first, voc_s.second, r);
			auto m = sim::helpers::_u<unsigned long>(voc_m.first, voc_m.second, r);
			j["t"]     = "voc";
			j["s"]     = s;
			j["m"]     = m;
			j["c"]     = sim::helpers::_u<unsigned>(voc_c.first, voc_c.second, r);
			j["b"]     = sim::helpers::_u<unsigned>(voc_b.first, voc_b.second, r);
			j["o"]     = sim::helpers::_u<unsigned long>(voc_o.first, voc_o.second, r);
			networks.emplace_back(i, s * m);

		} else if (network_type == sim::helpers::network_type::t3) {

			auto alpha = sim::helpers::_u<unsigned long>(t3_alpha.first, t3_alpha.second, r);
			auto beta = sim::helpers::_u<unsigned long>(t3_beta.first, t3_beta.second, r);
			auto gamma = sim::helpers::_u<unsigned long>(t3_gamma.first, t3_gamma.second, r);
			j["t"]     = "t3";
			j["alpha"] = alpha;
			j["beta"]  = beta;
			j["gamma"] = gamma;
			j["c"]     = sim::helpers::_u<unsigned>(t3_c.first, t3_c.second, r);
			j["b"]     = sim::helpers::_u<unsigned>(t3_b.first, t3_b.second, r);
			networks.emplace_back(i, alpha + beta + gamma);
		}

		return j;
	}

	json expand_network(unsigned long i, std::list<network>& networks, std::random_device& r)
	{
		std::pair<unsigned, unsigned> exp_n = std::make_pair(5, 20);
		auto n = sim::helpers::_u<unsigned>(exp_n.first, exp_n.second, r);
		auto net = std::begin(networks);
		json j {{"i", i}, {"r", "exp"}};

		do { net = random_element(networks, r); } while (net->n < n);

		*net += n;
		j["j"] = net->i, j["n"] = n;

		return j;
	}

	json contract_network(unsigned long i, std::list<network>& networks, std::random_device& r)
	{
		std::pair<unsigned, unsigned> con_n = std::make_pair(5, 20);
		auto n = sim::helpers::_u<unsigned>(con_n.first, con_n.second, r);
		auto net = std::begin(networks);
		json j {{"i", i}, {"r", "con"}};

		do { net = random_element(networks, r); } while (net->n < n);

		*net -= n;
		j["j"] = net->i, j["n"] = n;

		return j;
	}

	json delete_network(unsigned long i, std::list<network>& networks, std::random_device& r)
	{
		auto net = random_element(networks, r);
		json j {{"i", i}, {"r", "del"}, {"j", net->i}};
		networks.erase(net);
		return j;
	}
};

int main(int argc, const char** argv)
{
	return Generator{}(argc, argv);
}
