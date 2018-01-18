
#include <iostream>

#include "util/bipartite_matching.h"

int main(int argc, char** argv)
{

	std::vector<std::vector<double>> D = {
		{ 0, 1, 1, 2, 2, 2, 2 },
		{ 1, 0, 2, 1, 1, 3, 3 },
		{ 1, 2, 0, 3, 3, 1, 1 },
		{ 2, 1, 3, 0, 2, 4, 4 },
		{ 2, 1, 3, 2, 0, 4, 4 },
		{ 2, 3, 1, 4, 4, 0, 2 },
		{ 2, 3, 1, 4, 4, 2, 0 }
	};

// Sp -> [3]

	// for each sv e Sv
	// sv 1

		try {
			BipartiteMatching m;
			// for each sp e Sp
			m.add_candidate({1, 3, 0});
			BipartiteMatching::result r = m.find_matching();

			for (auto match : r.matches)
				std::cout << match.i << " -> " << match.j << " : " << match.cost << std::endl;
		} catch (...) { }

	// sv 2

		try {
			BipartiteMatching m;
			m.add_candidate({2, 3, 0});
			BipartiteMatching::result r = m.find_matching();

			for (auto match : r.matches)
				std::cout << match.i << " -> " << match.j << " : " << match.cost << std::endl;
		} catch (...) { }

	// for each xv e Xv
	// xv 1

		try {
			BipartiteMatching m;
			m.add_candidate({0, 0, (D[3][0] + D[3][0]) / 2});
			m.add_candidate({0, 1, (D[3][1] + D[3][1]) / 2});
			m.add_candidate({0, 2, (D[3][2] + D[3][2]) / 2});

			BipartiteMatching::result r = m.find_matching();

			for (auto match : r.matches)
				std::cout << match.i << " -> " << match.j << " : " << match.cost << std::endl;

		} catch (...) { }


	return 0;
}