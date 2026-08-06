#ifndef SAT_FORMULA_H
#define SAT_FORMULA_H

#include <vector>
#include "../search/task_proxy.h"
#include "../search/evaluator.h"

using Clause = std::vector<int>;
using Formula = std::vector<Clause>;

class sat_formula {
    public:
        TaskProxy task_proxy;
};

Formula build_sat_formula(TaskProxy task_proxy, int T);
Formula build_ehc_formula(State state, TaskProxy task_proxy, int T, bool with_constraint);
Formula initial_formula(Formula formula, TaskProxy task_proxy);
Formula transition_formula(Formula formula, TaskProxy task_proxy, int t);
Formula goal_formula(Formula formula, TaskProxy task_proxy, int T);
Formula better_formula_with_constraint(Formula formula, const State &current_state, TaskProxy task_proxy, int T);
Formula better_formula(Formula formula, const State &current_state, TaskProxy task_proxy, int T);
Formula actual_state_formula(Formula formula, State current_state, TaskProxy task_proxy);
Formula add_at_least_r(Formula formula, TaskProxy task_proxy, const std::vector<FactProxy> &goal_facts, int R, int T);
int lit_encoding(VariableProxy variable_proxy, TaskProxy task_proxy, int t);
int lit_encoding_op(OperatorProxy fact_proxy, TaskProxy task_proxy, int t);

#endif