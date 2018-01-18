#ifndef CLOUDSIM_VIRTUAL_RESOURCE_H
#define CLOUDSIM_VIRTUAL_RESOURCE_H

#include <initializer_list>
#include <vector>

#include "cloudsim/logic_error.h"

namespace cloudsim {

	class VirtualResource {

	public:
		VirtualResource() = delete;
		VirtualResource(unsigned demand, unsigned long group);
		VirtualResource(std::initializer_list<unsigned> demand, unsigned long group);
		VirtualResource(std::vector<unsigned> demand, unsigned long group);
		VirtualResource(const VirtualResource& copy_from) = default;
		unsigned demand(unsigned long i = 0) const throw (cloudsim::logic_error);
		unsigned long group() const;
		virtual ~VirtualResource();

	protected:
		std::vector<unsigned> _demand;
		unsigned long _group;
	};
}

#endif
