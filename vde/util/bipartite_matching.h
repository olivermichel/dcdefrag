
#ifndef VDE_BIPARTITE_MATCHING_H
#define VDE_BIPARTITE_MATCHING_H

#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>


class BipartiteMatching
{
public:

	struct pair {
		pair() = delete;
		pair(unsigned int i, unsigned int j, double cost);
		pair(const pair&) = default;
		unsigned int i;
		unsigned int j;
		double cost;
	};

	struct candidate : public pair {
		candidate(unsigned int i, unsigned int j, double cost) : pair(i, j, cost) { };
	};

	struct match : public pair {
		match(unsigned int i, unsigned int j, double cost) : pair(i, j, cost) { };
	};

	struct result {
		result();
		result(double cost, std::set<match> matches);
		result(const result&) = default;
		double cost;
		std::set<match> matches;
	};

	BipartiteMatching();

	void add_candidate(candidate c) throw(std::invalid_argument);

	unsigned count_candidates();

	BipartiteMatching::result find_matching() throw(std::logic_error, std::runtime_error);

private:

	void _pre_process() throw(std::logic_error);

	unsigned _m;
	unsigned _n;
	std::map<unsigned, unsigned> _i_k;
	std::map<unsigned, unsigned> _j_l;
	std::map<unsigned, unsigned> _k_i;
	std::map<unsigned, unsigned> _l_j;
	std::map<unsigned, candidate> _c;
	std::set<candidate> _input;
};


bool operator<(const BipartiteMatching::pair& a, const BipartiteMatching::pair& b);

std::ostream& operator<<(std::ostream& os, const BipartiteMatching::pair& pair);

std::ostream& operator<<(std::ostream& os, const BipartiteMatching::result& result);

#endif
