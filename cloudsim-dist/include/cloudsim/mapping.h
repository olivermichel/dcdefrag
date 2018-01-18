
#ifndef CLOUDSIM_MAPPING_H
#define CLOUDSIM_MAPPING_H

#include <vector>
#include <ostream>
#include <algorithm>

#include <json/json.h>

#include "physical_network.h"

namespace cloudsim {

	using json = nlohmann::json;
	class Mapping {

	public:

		template<typename T>
		class _map_proxy
		{
		public:

			_map_proxy()
				: _elements() {}

			unsigned long size() const
			{
				return _elements.size();
			}

			bool empty() const
			{
				return _elements.size() == 0;
			}

			void add(unsigned long i, T j)
				throw(cloudsim::logic_error)
			{
				if (_elements.find(i) != _elements.end())
					throw cloudsim::logic_error("Mapping::add: key already exists");

				_elements.insert(std::pair<unsigned long, T>(i, j));
			}

			bool has(unsigned long i)
			{
				return _elements.find(i) != _elements.end();
			}

			T operator[](unsigned long i)
				throw(cloudsim::logic_error)
			{
				if (!has(i))
					throw cloudsim::logic_error("Mapping::operator[]: key does not exist");

				return (*(_elements.find(i))).second;
			}

			typename std::map<unsigned long, T>::iterator begin()
			{
				return _elements.begin();
			}

			typename std::map<unsigned long, T>::iterator end()
			{
				return _elements.end();
			}

			json _to_json()
			{
				json j {json::array() };

				for (auto i : _elements)
					j.push_back({ i.first, i.second });

				return j;
			}

			friend class Mapping;

		private:

			std::map<unsigned long, T> _elements;
		};

		Mapping();

		double avg_path_length();

		std::vector<double> path_length_distribution();

		json _to_json();

		void _from_json(json& j);

		_map_proxy<unsigned long> servers;
		_map_proxy<unsigned long> switches;
		_map_proxy<std::vector<unsigned long>> links;
	};
}

#endif
