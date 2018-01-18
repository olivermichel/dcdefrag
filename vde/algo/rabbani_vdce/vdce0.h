
#ifndef ALGO_RABBANI_VDCE_VDCE0
#define ALGO_RABBANI_VDCE_VDCE0

#include <vector>
#include <map>
#include <stdexcept>
#include <ostream>

#include <cloudsim/virtual_network.h>
#include <cloudsim/physical_network.h>
#include <cloudsim/algorithm.h>
#include <cloudsim/mapping.h>
#include <cloudsim/runtime_error.h>

namespace algo {
	namespace rabbani_vdce {
		class VDCE0 : public cloudsim::Algorithm
		{
		public:

			const unsigned long TYPE_SWITCH = 0;
			const unsigned long TYPE_SERVER = 1;

			const unsigned long SERVER_CPU  = 0;
			const unsigned long SERVER_BW   = 1;

			const unsigned long SWITCH_BW   = 0;

			const unsigned long LINK_BW     = 0;

			VDCE0(cloudsim::PhysicalNetwork& p, cloudsim::VirtualNetwork& v,
				  std::vector<std::vector<int>>& d, bool debug = false,
				  std::ostream& os = std::cout);

			cloudsim::Mapping operator()() throw(cloudsim::runtime_error);

		private:
			std::vector<unsigned long> _Sp, _Xp, _Ep, _Sv, _Xv, _Ev;

			std::vector<std::vector<int>> _d;

			std::map<unsigned long, unsigned long> _vm_map(std::vector<unsigned long> Sps)
				throw(cloudsim::runtime_error);

			std::map<unsigned long, unsigned long>
				_sw_map(std::map<unsigned long, unsigned long>& vm_map)
				throw(cloudsim::runtime_error);

			std::map<unsigned long, std::vector<unsigned long>>
			_link_map(std::map<unsigned long, unsigned long>& map)
				throw(cloudsim::runtime_error);

			double _a(unsigned long i, unsigned long j) const;

			double _b(unsigned long i, unsigned long j, std::map<unsigned long, unsigned long>& s_map) const;
		};
	}
}

#endif