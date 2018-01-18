
#include "cloudsim/touchable.h"

cloudsim::interfaces::Touchable::Touchable()
	: _touched(false)
{ }

void cloudsim::interfaces::Touchable::touch() throw(std::logic_error)
{
	if (_touched)
		throw std::logic_error("already touched");

	_touched = true;
}

void cloudsim::interfaces::Touchable::untouch() throw(std::logic_error)
{
	if (!_touched)
		throw std::logic_error("not touched");

	_touched = false;
}

bool cloudsim::interfaces::Touchable::touched()
{
	return _touched;
}
