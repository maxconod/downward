
#include "sat_planning.h"
#include "../search/utils/logging.h"
#include "../search/search_algorithms/search_common.h"

#include "cadical.hpp"
#include "sat_formula.h"
#include <cassert>

#include "../search/evaluator.h"

better_state find_better_state(State state, TaskProxy task_proxy, Evaluator * heuristic, AbstractTask &abstract_task) {
    int T;
    State new_state = state;
    std::vector<OperatorID> plan;

    for (T = 1; T < 30; T++) {
        CaDiCaL::Solver * solver = new CaDiCaL::Solver;
        solver->set("factor", 0);
        Formula formula = build_ehc_formula(new_state, task_proxy, T, heuristic);

        for (Clause clause : formula) {
            for (int lit : clause) {
                solver->add(lit);
            }
            solver->add(0);
        }

        int result = solver->solve();
        
        // The new state is the goal state
        //if (is_goal(new_state, task_proxy)) {
        //    return new_state;
        //}

        utils::g_log << "looping through the find better state at T=" << T << std::endl;

        // UNKNOWN
        if (result == 0) {
            utils::g_log << "error" << std::endl;
            // return error
        }
        // SAT
        else if (result == 10) {
            // We return the new state if it has a better h than the previous state
            utils::g_log << "the solver found a solution" << std::endl;
            new_state = extract_state(solver, task_proxy, T, abstract_task);
            plan = extract_plan(solver, task_proxy, T);
            //if (h_goal_count(new_state, task_proxy) < h_goal_count(state, task_proxy)) {
            //    return new_state;
            //}
            //break;
            assert(h_goal_count(new_state, task_proxy) < h_goal_count(state, task_proxy));
            return {new_state, plan};
        }
        // UNSAT
        else if (result == 20) {
            utils::g_log << "the solver didn't find a solution at T=" << T << std::endl;
        }

    }
    return {state, plan};
}

Model solve_formula(State current_state, TaskProxy task_proxy, Evaluator * heuristic) {
    CaDiCaL::Solver * solver = new CaDiCaL::Solver;
    Model model = {std::nullopt, 0, 0};
    int result;
    int T;

    for (T = 0; T < 1000; T++) {
        Formula formula = build_sat_formula(task_proxy, T);

        // Using the dimacs format
        for (Clause clause : formula) {
            for (int lit : clause) {
                solver->add(lit);
            }
            solver->add(0);
        }

        result = solver->solve();

        // UNKNOWN
        if (result == 0) {
            return model;
        }
        // SATISFIABLE
        else if (result == 10) {
            break;
        }
        else if (result == 20) {

        }
        else {
            return model;
        }
    }

    if (result == 10) {
        model = {task_proxy, T, result};
    }

    return model;
}

// Goal-count heuristic
int h_goal_count(State state, TaskProxy task) {
    GoalsProxy goal_state = task.get_goals();
    int count = 0;

    for (FactProxy fact : goal_state) {
        if (state[fact.get_variable()] != fact) {
            count++;
        }
    }

    return count;
}

State extract_state(CaDiCaL::Solver * solver, TaskProxy task_proxy, int t, const AbstractTask &abstract_task) {
    std::vector<int> values(task_proxy.get_variables().size());

    for (VariableProxy var : task_proxy.get_variables()) {
        int lit = lit_encoding(var, task_proxy, t);

        if (solver->val(lit) > 0) {
            values[var.get_id()] = 0;
        }
        else {
            values[var.get_id()] = 1;
        }
    }

    return State(abstract_task, std::move(values));
}

bool is_goal(State state, TaskProxy task_proxy) {
    for (FactProxy fact : task_proxy.get_goals()) {
        if (state[fact.get_variable()] != fact) {
            return false;
        }
    }
    return true;
}

std::vector<OperatorID> extract_plan(CaDiCaL::Solver *solver, TaskProxy task_proxy, int T) {
    std::vector<OperatorID> actual_plan;

    for (int t = 0; t < T; t++) {
        for (OperatorProxy op : task_proxy.get_operators()) {
            if (solver->val(lit_encoding_op(op, task_proxy, t)) > 0) {
                actual_plan.push_back(OperatorID(op.get_id()));
                break;
            }
        }
    }
    return actual_plan;
}