#ifndef CLOUDSIM_PHYSICAL_RESOURCE_H
#define CLOUDSIM_PHYSICAL_RESOURCE_H

#include <vector>
#include <initializer_list>

#include "cloudsim/logic_error.h"

namespace cloudsim {

	class PhysicalResource {

	public:
		PhysicalResource() = delete;
		PhysicalResource(unsigned capacity, unsigned long group);
		PhysicalResource(std::initializer_list<unsigned> capacity, unsigned long group);
		PhysicalResource(std::vector<unsigned> capacity, unsigned long group);
		PhysicalResource(const PhysicalResource& copy_from) = default;
		unsigned capacity(unsigned long i = 0) const throw (cloudsim::logic_error);
		unsigned long group() const;
		virtual ~PhysicalResource();

	protected:
		std::vector<unsigned> _capacity;
		unsigned long _group;
	};
}

#endif
