
#include "cloudsim/mapping.h"

using json = nlohmann::json;

cloudsim::Mapping::Mapping()
	: servers(), switches(), links() { }

double cloudsim::Mapping::avg_path_length()
{
	return std::accumulate(std::begin(links._elements), std::end(links._elements), 0,
		[] (unsigned long a, std::pair<unsigned long, std::vector<unsigned long>> p) {
			return a + p.second.size();
	   	}) / links.size();
}

std::vector<double> cloudsim::Mapping::path_length_distribution()
{
	std::vector<double> d(links.size(), 0);

	std::transform(std::begin(links._elements), std::end(links._elements), std::begin(d),
		[] (std::pair<unsigned long, std::vector<unsigned long>> p) {
			return (double) p.second.size();
	});

	return d;
}

json cloudsim::Mapping::_to_json()
{
	json j {{"servers", servers._to_json() },
			{"switches", switches._to_json() },
			{"links", links._to_json() }};

	return j;
}

void cloudsim::Mapping::_from_json(json& j)
{
	for (auto i : j["servers"])
		servers.add(i[0], i[1]);

	for (auto i : j["switches"])
		switches.add(i[0], i[1]);

	for (auto i : j["links"])
		links.add(i[0], i[1]);
}
