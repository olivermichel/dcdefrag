#ifndef CLOUDSIM_TOUCHABLE_H
#define CLOUDSIM_TOUCHABLE_H

#include <stdexcept>

namespace cloudsim {
	namespace interfaces {
		class Touchable
		{
		public:

			Touchable();
			Touchable(const Touchable& copy_from) = default;
			void touch() throw(std::logic_error);
			void untouch() throw(std::logic_error);
			bool touched();

		private:
			bool _touched;
		};
	}
}

#endif
