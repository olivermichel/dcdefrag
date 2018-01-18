
#ifndef CLOUDSIM_RESERVABLE_H
#define CLOUDSIM_RESERVABLE_H

#include <stdexcept>
#include <initializer_list>
#include <vector>
#include "logic_error.h"

namespace cloudsim {
	namespace interfaces {
		class Reservable
		{
		public:
			Reservable() = delete;

			Reservable(unsigned w);

			Reservable(std::initializer_list<unsigned> w);

			Reservable(std::vector<unsigned> w);

			Reservable(const Reservable& other) = default;

			Reservable& operator=(const Reservable& other) = default;

			void reserve(unsigned w, unsigned long i = 0)
				throw (cloudsim::logic_error);

			void release(unsigned w, std::vector<unsigned>::size_type i = 0)
				throw (cloudsim::logic_error);

			void block_all() noexcept;

			void block(unsigned w, std::vector<unsigned>::size_type i = 0)
				throw (cloudsim::logic_error);

			void reset_reservation(std::vector<unsigned>::size_type i = 0)
				throw (cloudsim::logic_error);

			void reset_all() noexcept;

			unsigned residual(std::vector<unsigned>::size_type i = 0) const
				throw (cloudsim::logic_error);

			unsigned utilization(std::vector<unsigned>::size_type i = 0) const
				throw (cloudsim::logic_error);

			unsigned reservation(std::vector<unsigned>::size_type i = 0) const
				throw (cloudsim::logic_error);

			std::size_t count_properties() const noexcept;

		protected:
			std::vector<unsigned> _weight;
			std::vector<unsigned> _utilization;
			std::vector<unsigned> _reservation;

			std::size_t _property_count;

			void _check_index(std::vector<unsigned>::size_type i) const
				throw (cloudsim::logic_error);
		};
	}
}

#endif
