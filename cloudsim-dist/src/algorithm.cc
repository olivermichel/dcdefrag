
#include "cloudsim/algorithm.h"

cloudsim::Algorithm::Algorithm(PhysicalNetwork& p, VirtualNetwork& v, bool debug, std::ostream& os)
	: _P(p), _V(v), _debug(debug), _os(os) { }
