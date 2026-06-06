#ifndef SAT_FORMULA_H
#define SAT_FORMULA_H

#include <vector>
#include "../search/task_proxy.h"

using Clause  = std::vector<int>;
using Formula = std::vector<Clause>;

Formula build_sat_formula(TaskProxy task, int T);

Formula initial_formula(Formula formula, TaskProxy task);

Formula transition_formula(Formula formula, TaskProxy task, int T);

Formula goal_formula(Formula formula, TaskProxy task);

#endif