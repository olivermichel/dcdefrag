#ifndef CLOUDSIM_UTIL_H
#define CLOUDSIM_UTIL_H

#include <numeric>

namespace cloudsim {
	namespace util {

		template<typename T>
		double mean(std::vector<T> data)
		{
			return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
		}

		template<typename T>
		double variance(std::vector<T> data)
		{
			double mean = util::mean<T>(data);
			T temp = 0;

			for (auto i : data)
				temp += ((i - mean) * (i - mean));

			return temp / data.size();
		}
	}
}



#endif
