
#ifndef FAST_DOWNWARD_SAT_PLANNING_H
#define FAST_DOWNWARD_SAT_PLANNING_H

#include <optional>
#include <vector>
#include <tuple>
#include "sat_formula.h"

std::optional<std::vector<int>> solve_formula(const Formula &formula, int num_vars);
std::tuple<Plan, T, I> extract_plan();


#endif
