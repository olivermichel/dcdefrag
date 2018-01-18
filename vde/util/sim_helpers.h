
#ifndef VDE_HELPERS_H
#define VDE_HELPERS_H

#include <stdexcept>
#include <memory>
#include <random>
#include <cloudsim/physical_network.h>

namespace sim {
	namespace helpers {

		void debug_pn(cloudsim::PhysicalNetwork& p)
		{


			std::cout << std::setw(3) << "i" << std::setw(3) << "t" << std::setw(10) << "c"
					  << std::setw(10) << "u" << std::setw(10) << "r" << std::endl;
			std::cout
				<< "---------------------------------------------------------------------------------"
				<< std::endl;

			for (auto a = p.nodes.begin(); a != p.nodes.end(); a++) {
				cloudsim::PhysicalNode* node = dynamic_cast<cloudsim::PhysicalNode*>(*a);
				std::cout << std::setw(3) << a.id()
						  << std::setw(3) << node->type()
						  << std::setw(10) << (std::to_string((int) node->capacity(0))
											   + "/" + std::to_string((int) node->capacity(1)))
						  << std::setw(10) << (std::to_string((int) node->utilization(0))
											   + "/" + std::to_string((int) node->utilization(1)))
						  << std::setw(10) << (std::to_string((int) node->reservation(0))
											   + "/" + std::to_string((int) node->reservation(1)));

				if (!a.sub_vertices().empty()) {
					std::cout << "  [ ";
					for (auto b : a.sub_vertices()) {
						cloudsim::VirtualNode* v = dynamic_cast<cloudsim::VirtualNode*>(*b);
						std::cout << v->demand(0) << "/" << v->demand(1) << " ";

					}
					std::cout << "]";
				}
				std::cout << std::endl;
			}

			std::cout
				<< "---------------------------------------------------------------------------------"
				<< std::endl;

			for (auto a = p.edges.begin(); a != p.edges.end(); a++) {
				cloudsim::PhysicalEdge* edge = dynamic_cast<cloudsim::PhysicalEdge*>(*a);
				std::cout << std::setw(3) << a.id()
						  << std::setw(13) << edge->capacity(0)
						  << std::setw(10) << edge->utilization(0)
						  << std::setw(10) << edge->reservation(0);

				if (!a.sub_edges().empty()) {
					std::cout << "  [ ";
					for (auto b : a.sub_edges()) {
						cloudsim::VirtualEdge* v = dynamic_cast<cloudsim::VirtualEdge*>(*b);
						std::cout << v->demand(0) << " ";

					}
					std::cout << "]";
				}
				std::cout << std::endl;
			}

			std::cout << std::flush;
		}

		template<typename T>
		T _u(T a, T b, std::random_device& r)
		{
			std::uniform_int_distribution<> d(a, b);
			return (T) d(r);
		}

		std::unique_ptr<cloudsim::VirtualNetwork> vc(unsigned long r, unsigned long n, unsigned c,
													 unsigned b)
		{
			// r - request identifier
			// n - # virtual machines
			// c - capacity at VMs
			// b - bandwidth at VMs

			std::unique_ptr<cloudsim::VirtualNetwork> v(new cloudsim::VirtualNetwork(r, 0));

			v->nodes.add(
				new cloudsim::VirtualNode{0, {(unsigned) n * b, (unsigned) n * b}, 0, 0, v.get()});

			for (unsigned i = 1; i <= n; i++) {
				v->nodes.add(new cloudsim::VirtualNode{i, {c, b}, 1, 1, v.get()});
				v->edges.add(v->nodes[0], v->nodes[i],
					new cloudsim::VirtualEdge{i - 1, b, 0, v.get()});
			}
			return v;
		}

		std::unique_ptr<cloudsim::VirtualNetwork> voc(unsigned long r, unsigned long s,
													  unsigned long m, unsigned c, unsigned b,
													  unsigned long o)
		{
			// r - request identifier
			// n - # virtual machines
			// s - group size
			// c - capacity at VMs
			// b - bandwidth at VMs
			// o - oversubscription

			unsigned long n = s * m;

			std::unique_ptr<cloudsim::VirtualNetwork> v(new cloudsim::VirtualNetwork(r, 1));

			v->nodes.add(
				new cloudsim::VirtualNode{0, {(unsigned) (n * b / o), (unsigned) (n * b / o)}, 0, 0,
										  v.get()});

			for (unsigned i = 1; i <= n / s + 1; i++) {
				v->nodes.add(
					new cloudsim::VirtualNode{i, {(unsigned) s * b, (unsigned) s * b}, 0, 1,
											  v.get()});
				v->edges.add(v->nodes[0], v->nodes[i],
							 new cloudsim::VirtualEdge{i - 1, (unsigned) (b * s / o), 0, v.get()});
			}

			for (unsigned i = 1; i <= n / s + 1; i++) {
				for (unsigned j = 0; j < s; j++) {
					v->nodes.add(
						new cloudsim::VirtualNode{2 + j + n / s - s + i * s, {c, b}, 1, 2,v.get()});
					v->edges.add(v->nodes[i], v->nodes[1 + j + n / s - s + i * s],
						new cloudsim::VirtualEdge{j + n / s - s + i * s, b, 0, v.get()});
				}
			}

			return v;
		}

std::unique_ptr<cloudsim::VirtualNetwork> t3(unsigned long r, unsigned long alpha,
											 unsigned long beta, unsigned long gamma, unsigned c,
											 unsigned b)
		{
			// r     - request identifier
			// alpha - # virtual machines in first group
			// beta  - # virtual machines in second group
			// gamma - # virtual machines in third group
			// c     - capacity at VMs
			// b     - bandwidth at VMs

			std::unique_ptr<cloudsim::VirtualNetwork> v(new cloudsim::VirtualNetwork(r, 2));

			unsigned long m = 0, d1 = 0, d2 = 0;
			d1 = d2 = alpha * b + beta * b;
			v->nodes.add(
				new cloudsim::VirtualNode{0, { (unsigned) d1, (unsigned) d2 } , 0, 0, v.get()});
			v->nodes.add(
				new cloudsim::VirtualNode{1, { (unsigned) d1, (unsigned) d2 }, 0, 0, v.get()});

			for (unsigned i = 1; i <= alpha; i++) {
				v->nodes.add(new cloudsim::VirtualNode{ i + 1, { c, b }, 1, 1, v.get() });
				v->edges.add(v->nodes[i+1], v->nodes[0],
					new cloudsim::VirtualEdge{ m++, b, 0, v.get()});
			}


			for (unsigned i = 1; i <= beta; i++) {
				v->nodes.add(
					new cloudsim::VirtualNode{ i + alpha + 1, { c, b * 2}, 1, 1, v.get() });
				v->edges.add(v->nodes[i + alpha + 1], v->nodes[0],
							 new cloudsim::VirtualEdge{ m++, b, 0, v.get()});
				v->edges.add(v->nodes[i + alpha + 1], v->nodes[1],
							 new cloudsim::VirtualEdge{ m++, b, 0, v.get()});
			}


			for (unsigned i = 1; i <= gamma; i++) {
				v->nodes.add(
					new cloudsim::VirtualNode{ i + alpha + beta + 1, { c, b }, 1, 1, v.get() });
				v->edges.add(v->nodes[i + alpha + beta + 1], v->nodes[1],
							 new cloudsim::VirtualEdge{ m++, b, 0, v.get()});
			}

			return v;
		}

		std::unique_ptr<cloudsim::VirtualNetwork> new_network(unsigned long i, std::random_device& r)
		{
			// VC:
			std::pair<unsigned, unsigned> vc_n = std::make_pair(10, 45);
			std::pair<unsigned, unsigned> vc_c = std::make_pair(50, 150);
			std::pair<unsigned, unsigned> vc_b = std::make_pair(50, 150);

			// VOC: n = s * m
			std::pair<unsigned, unsigned> voc_s = std::make_pair(2, 6);
			std::pair<unsigned, unsigned> voc_m = std::make_pair(2, 6);
			std::pair<unsigned, unsigned> voc_o = std::make_pair(1, 4);
			std::pair<unsigned, unsigned> voc_c = std::make_pair(50, 150);
			std::pair<unsigned, unsigned> voc_b = std::make_pair(50, 150);

			// T3
			std::pair<unsigned, unsigned> t3_alpha = std::make_pair(5, 15);
			std::pair<unsigned, unsigned> t3_beta = std::make_pair(5, 15);
			std::pair<unsigned, unsigned> t3_gamma = std::make_pair(5, 15);
			std::pair<unsigned, unsigned> t3_c = std::make_pair(50, 150);
			std::pair<unsigned, unsigned> t3_b = std::make_pair(50, 150);

			// VC, VOC, 3T
			std::discrete_distribution<> network_type_dist{1, 1, 1};

			int type = network_type_dist(r);

			switch (type) {
				case 0:
					return vc(i,
						  _u<unsigned>(vc_n.first, vc_n.second, r),
						  _u<unsigned>(vc_c.first, vc_c.second, r),
						  _u<unsigned>(vc_b.first, vc_b.second, r)
					);

				case 1:
					return voc(i,
						   _u<unsigned long>(voc_s.first, voc_s.second, r),
						   _u<unsigned long>(voc_m.first, voc_m.second, r),
						   _u<unsigned>(voc_c.first, voc_c.second, r),
						   _u<unsigned>(voc_b.first, voc_b.second, r),
						   _u<unsigned long>(voc_o.first, voc_o.second, r)
					);
				case 2:
					return t3(i,
							_u<unsigned long>(t3_alpha.first, t3_alpha.second, r),
							_u<unsigned long>(t3_beta.first, t3_beta.second, r),
							_u<unsigned long>(t3_gamma.first, t3_gamma.second, r),
							_u<unsigned>(t3_c.first, t3_c.second, r),
							_u<unsigned>(t3_b.first, t3_b.second, r)
					);
				default: break;
			}

			return nullptr;
		}

		enum class request_type : int
		{
			unknown          = -1,
			new_network      =  0,
			expand_network   =  1,
			contract_network =  2,
			delete_network   =  3
		};

		request_type request_type(std::string s)
		{
			if (s == "new")
				return request_type::new_network;
			else if (s == "exp")
				return request_type::expand_network;
			else if (s == "con")
				return request_type::contract_network;
			else if (s == "del")
				return request_type::delete_network;
			else
				return request_type::unknown;
		}

		enum class network_type : int
		{
			unknown = -1,
			vc      =  0,
			voc     =  1,
			t3      =  2
		};

		network_type network_type(std::string s)
		{
			if (s == "vc")
				return network_type::vc;
			else if (s == "voc")
				return network_type::voc;
			else if (s == "t3")
				return network_type::t3;
			else
				return network_type::unknown;
		}
	}
}
#endif
