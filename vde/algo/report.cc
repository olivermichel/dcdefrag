#include "report.h"

#include <json/json.h>
#include <fstream>

void algo::write_report(std::list<report> reports, std::string file_name)
{
	nlohmann::json j;

	for (algo::report r : reports) {
		j.push_back({{"success", r.s},
					 {"path_len", r.path_len},
					 {"t", r.t},
					 {"server_util", r.server_util},
					 {"switch_util", r.switch_util},
					 {"edge_util", r.edge_util}
					});
	}

	std::ofstream o(file_name);
	o << j << std::endl;
	o.close();
}