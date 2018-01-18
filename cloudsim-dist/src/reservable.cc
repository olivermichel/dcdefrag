
#include <cloudsim/logic_error.h>
#include "cloudsim/reservable.h"

cloudsim::interfaces::Reservable::Reservable(unsigned w)
	: _weight({w}),
	  _utilization({0}),
	  _reservation({0}),
	  _property_count(1)
{ }

cloudsim::interfaces::Reservable::Reservable(std::initializer_list<unsigned> w)
	: _weight(w),
	  _utilization(w.size(), 0),
	  _reservation(w.size(), 0),
	  _property_count(w.size())
{ }

cloudsim::interfaces::Reservable::Reservable(std::vector<unsigned> w)
	: _weight(w),
	  _utilization(w.size(), 0),
	  _reservation(w.size(), 0),
	  _property_count(w.size())
{ }

void cloudsim::interfaces::Reservable::reserve(unsigned w, unsigned long i)
	throw(cloudsim::logic_error)
{
	_check_index(i);

	if (w > (_weight[i] - _utilization[i]) || _reservation[i] + w > _weight[i])
		throw cloudsim::logic_error("Reservable::reserve(): not enough capacity");

	_reservation[i] += w;
}

void cloudsim::interfaces::Reservable::release(unsigned w, std::vector<unsigned>::size_type i)
	throw(cloudsim::logic_error)
{

	_check_index(i);

	if (_utilization[i] == 0)
		throw cloudsim::logic_error("Reservable::release(): is not utilized");

//	if (_reservation[i] != 0)
//		throw cloudsim::logic_error(
//			"Reservable::release(" + std::to_string(w) + "," + std::to_string(i) + "): existing reservation on node"
//		);

	if (_utilization[i] - w < 0)
		throw cloudsim::logic_error("Reservable::release(): not enough weight to release");

	_utilization[i] -= w;
}

void cloudsim::interfaces::Reservable::block(unsigned w, std::vector<unsigned>::size_type i)
	throw(cloudsim::logic_error)
{
	_check_index(i);

	if (w <= 0)
		throw cloudsim::logic_error("Reservable::block(): w must be >= 0");

	if (_reservation[i] > 0)
		if (w > _reservation[i])
			throw cloudsim::logic_error("Reservable::block(): can't block more than reserved");
		else
			_utilization[i] += w, _reservation[i] -= w;
	else
		if (w > (_weight[i] - _utilization[i]))
			throw cloudsim::logic_error("Reservable::block(): not enough weight");
		else
			_utilization[i] += w;
}

void cloudsim::interfaces::Reservable::block_all() noexcept
{
	for (unsigned long i = 0; i < _property_count; i++)
		_utilization[i] += _reservation[i], _reservation[i] = 0;
}

void cloudsim::interfaces::Reservable::reset_reservation(std::vector<unsigned>::size_type i)
	throw(cloudsim::logic_error)
{
	_check_index(i);

	if (!_reservation[i])
		throw cloudsim::logic_error("Reservable::reset_reservation(): no reservation");

	_reservation[i] = 0;
}

void cloudsim::interfaces::Reservable::reset_all() noexcept
{
	for (unsigned i = 0; i < _property_count; i++)
		_reservation[i] = 0;
}

unsigned cloudsim::interfaces::Reservable::residual(std::vector<unsigned>::size_type i) const
	throw(cloudsim::logic_error)
{
	_check_index(i);
	return _weight[i] - _utilization[i] - _reservation[i];
}

unsigned cloudsim::interfaces::Reservable::utilization(std::vector<unsigned>::size_type i) const
	throw(cloudsim::logic_error)
{
	_check_index(i);
	return _utilization[i];
}

unsigned cloudsim::interfaces::Reservable::reservation(std::vector<unsigned>::size_type i) const
	throw(cloudsim::logic_error)
{
	_check_index(i);
	return _reservation[i];
}

std::size_t cloudsim::interfaces::Reservable::count_properties() const noexcept
{
	return _property_count;
}

void  cloudsim::interfaces::Reservable::_check_index(std::vector<unsigned>::size_type i) const
	throw (cloudsim::logic_error)
{
	if (i >= _property_count)
		throw cloudsim::logic_error("Reservable::_check_index(): invalid index");
}
