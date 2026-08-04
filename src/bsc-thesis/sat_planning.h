
#ifndef FAST_DOWNWARD_SAT_PLANNING_H
#define FAST_DOWNWARD_SAT_PLANNING_H

#include <optional>
#include <vector>
#include <tuple>
#include "sat_formula.h"
#include "../search/evaluator.h"
#include "../search/sat_ehc.h"
#include "cadical.hpp"

class sat_planning {
public:
    TaskProxy task_proxy;
};

struct Model {
    std::vector<OperatorID> plan;
    int T;
    int result;
};

struct better_state {
    State state;
    std::vector<OperatorID> plan;
};

Model solve_formula(State current_state, TaskProxy task_proxy);
better_state find_better_state(State current_state, TaskProxy task_proxy, Evaluator * heuristic, AbstractTask &abstract_task, SearchStatistics &statistics, bool using_constraint);
State extract_state(CaDiCaL::Solver * solver, TaskProxy task_proxy, int T, const AbstractTask &abstract_task);
std::vector<OperatorID> extract_plan(CaDiCaL::Solver *solver, TaskProxy task_proxy, int T);
Evaluator get_heuristic(State state);
int h_goal_count(State state, TaskProxy task);
bool is_goal(State state, TaskProxy task_proxy);
// std::tuple<Plan, T, I> extract_plan();



#endif
