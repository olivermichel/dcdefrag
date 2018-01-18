
#ifndef CLOUDSIM_GREEDY_H
#define CLOUDSIM_GREEDY_H

#include "algorithm.h"
#include "physical_network.h"
#include "virtual_network.h"
#include "runtime_error.h"
#include <ostream>

namespace cloudsim {
	class Greedy : public Algorithm {
	public:
		Greedy(PhysicalNetwork& p, VirtualNetwork& v, bool debug = false,
			   std::ostream& os = std::cout);
		Mapping operator()() throw (cloudsim::runtime_error);
	};
}

#endif
