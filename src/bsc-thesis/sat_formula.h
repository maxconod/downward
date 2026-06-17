#ifndef SAT_FORMULA_H
#define SAT_FORMULA_H

#include <vector>
#include "../search/task_proxy.h"

using Clause = std::vector<int>;
using Formula = std::vector<Clause>;

class sat_formula {
    TaskProxy task_proxy;
};

Formula build_sat_formula(TaskProxy task_proxy, int T);
Formula initial_formula(Formula formula, TaskProxy task_proxy, int T);
Formula transition_formula(Formula formula, TaskProxy task_proxy, int T, int t);
Formula goal_formula(Formula formula, TaskProxy task_proxy, int T);
// int lit_encoding(FactProxy fact_proxy, int T, int t);

#endif