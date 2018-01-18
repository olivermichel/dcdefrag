
#ifndef CLOUDSIM_ALGORITHM_H
#define CLOUDSIM_ALGORITHM_H

#include <stdexcept>
#include "physical_network.h"
#include "virtual_network.h"
#include "mapping.h"
#include "runtime_error.h"

namespace cloudsim {
	class Algorithm
	{
	public:
		Algorithm(PhysicalNetwork& p, VirtualNetwork& v, bool debug = false,
				  std::ostream& os = std::cout);
		virtual Mapping operator()() throw (runtime_error) = 0;
	protected:
		PhysicalNetwork& _P;
		VirtualNetwork& _V;
		bool _debug;
		std::ostream& _os;
	};
}

#endif
