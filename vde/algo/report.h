#ifndef VDE_ALGO_REPORT_H
#define VDE_ALGO_REPORT_H

#include <list>
#include <vector>
#include <string>

namespace algo {
	struct report {
		report(unsigned long i) : i(i) { }
		unsigned long i;
		bool s;
		double path_len;
		double t;
		std::vector<double> server_util;
		std::vector<double> switch_util;
		std::vector<double> edge_util;
	};

	void write_report(std::list<report> reports, std::string file_name);
};

#endif