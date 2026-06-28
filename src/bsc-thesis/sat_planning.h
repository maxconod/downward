
#ifndef FAST_DOWNWARD_SAT_PLANNING_H
#define FAST_DOWNWARD_SAT_PLANNING_H

#include <optional>
#include <vector>
#include <tuple>
#include "sat_formula.h"
#include "../search/evaluator.h"

class sat_planning {
public:
    TaskProxy task_proxy;
};

struct Model {
    std::optional<TaskProxy> task_proxy;
    int T;
    int result;
};

Model solve_formula(State current_state, TaskProxy task_proxy, Evaluator * heuristic);
State find_better_state(State current_state, TaskProxy task_proxy, Evaluator * heuristic);
Evaluator get_heuristic(State state);
// std::tuple<Plan, T, I> extract_plan();



#endif
