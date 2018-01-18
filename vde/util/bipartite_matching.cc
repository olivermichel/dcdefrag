
#include "bipartite_matching.h"
#include <gurobi_c++.h>

BipartiteMatching::BipartiteMatching()
	: _m(0), _n(0), _i_k(), _j_l(), _k_i(), _l_j(), _c(), _input() { }

void BipartiteMatching::add_candidate(candidate c)
throw(std::invalid_argument)
{
	if (_input.find(c) == _input.end())
		_input.insert(c);
	else
		throw std::invalid_argument("already exists");
}

unsigned BipartiteMatching::count_candidates()
{
	return (unsigned int) _input.size();
}

BipartiteMatching::result BipartiteMatching::find_matching()
throw(std::logic_error, std::runtime_error)
{
	GRBEnv* env = nullptr;
	GRBVar *inflow = nullptr, *outflow = nullptr, *flow = nullptr;
	double *inflow_lb = nullptr, *inflow_ub = nullptr, *inflow_weights = nullptr;
	double *outflow_lb = nullptr, *outflow_ub = nullptr, *outflow_weights = nullptr;
	double *flow_lb = nullptr,  *flow_ub = nullptr, *flow_weights = nullptr;

	_pre_process();

	std::vector<std::vector<int>> flow_cons_a(_m, std::vector<int>(_n, -1));
	std::vector<std::vector<int>> flow_cons_b(_n, std::vector<int>(_m, -1));
	std::set<match> matches;
	double cost = -1;

	try {
		env = new GRBEnv();
		env->set(GRB_IntParam_LogToConsole, 0);
		GRBModel model = GRBModel(*env);

		// inflow variables
		inflow_lb = new double[_m], inflow_ub = new double[_m];
		inflow_weights = new double[_m];

		for (int i = 0; i < _m; ++i)
			inflow_lb[i] = 0, inflow_ub[i] = 1, inflow_weights[i] = 0;

		outflow_lb = new double[_n], outflow_ub = new double[_n];
		outflow_weights = new double[_n];

		for (int i = 0; i < _n; ++i)
			outflow_lb[i] = 0, outflow_ub[i] = 1, outflow_weights[i] = 0;

		flow_lb = new double[_c.size()], flow_ub = new double[_c.size()];
		flow_weights = new double[_c.size()];

		for (auto& c : _c) {
			flow_lb[c.first] = 0, flow_ub[c.first] = 1;
			flow_weights[c.first] = (double) c.second.cost;
			flow_cons_a[c.second.i][c.second.j] = c.first;
			flow_cons_b[c.second.j][c.second.i] = c.first;
		}

		inflow = model.addVars(inflow_lb, inflow_ub, inflow_weights, 0, nullptr, _m);
		outflow = model.addVars(outflow_lb, outflow_ub, outflow_weights, 0, nullptr, _n);
		flow = model.addVars(flow_lb, flow_ub, flow_weights, 0, nullptr, (unsigned) _c.size());

		for (unsigned i = 0; i < _m; ++i) {
			GRBLinExpr a_out = 0;
			for (unsigned j = 0; j < _n; ++j)
				if (flow_cons_a[i][j] >= 0)
					a_out += flow[flow_cons_a[i][j]];

			model.addConstr(inflow[i] == a_out);
		}

		for (unsigned i = 0; i < _n; ++i) {
			GRBLinExpr b_in = 0;
			for (unsigned j = 0; j < _m; ++j)
				if (flow_cons_b[i][j] >= 0)
					b_in += flow[flow_cons_b[i][j]];
			model.addConstr(b_in == outflow[i]);
		}

		GRBLinExpr out = 0;
		for (unsigned i = 0; i < _n; ++i)
			out += outflow[i];

		model.addConstr(out == _m);

		model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
		model.optimize();

		if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
			cost = model.get(GRB_DoubleAttr_ObjVal);

			for (auto c : _c)
				if (flow[c.first].get(GRB_DoubleAttr_X) > 0.0001)
					matches.insert(match(_k_i[c.second.i], _l_j[c.second.j], c.second.cost));

		} else {
			throw std::runtime_error("no solution found");
		}
	} catch (GRBException& e) {
		throw std::runtime_error(e.getMessage());
	} catch (...) {
		throw std::runtime_error("some other error");
	}

	delete env;
	delete inflow, delete outflow, delete flow;
	delete inflow_lb, delete inflow_ub, delete inflow_weights;
	delete outflow_lb, delete outflow_ub, delete outflow_weights;
	delete flow_lb, delete flow_ub, delete flow_weights;

	return result(cost, matches);
}

BipartiteMatching::pair::pair(unsigned int i, unsigned int j, double cost)
	: i(i), j(j), cost(cost) { }


BipartiteMatching::result::result()
	: cost(-1), matches() { }

BipartiteMatching::result::result(double cost, std::set<match> matches)
	: cost(cost), matches(matches) { }

void BipartiteMatching::_pre_process() throw(std::logic_error)
{
	unsigned k = 0, l = 0, z = 0;

	for (auto c : _input) { // std::set<candidate>

		if (_i_k.find(c.i) == _i_k.end())
			_i_k[c.i] = k, _k_i[k++] = c.i;

		if (_j_l.find(c.j) == _j_l.end())
			_j_l[c.j] = l, _l_j[l++] = c.j;

		_c.emplace(z++, candidate(_i_k[c.i], _j_l[c.j], c.cost));
	}

	if (k > l)
		throw std::logic_error("m must be <= n");

	_m = k, _n = l;
}

bool operator<(const BipartiteMatching::pair& a, const BipartiteMatching::pair& b)
{
	return a.i < b.i || a.j < b.j;
}

std::ostream& operator<<(std::ostream& os, const BipartiteMatching::pair& pair)
{
	return (os << pair.i << " -> " << pair.j << " / " << pair.cost);
}

std::ostream& operator<<(std::ostream& os, const BipartiteMatching::result& result)
{
	for (auto match : result.matches)
		os << match << std::endl;

	return os;
}
