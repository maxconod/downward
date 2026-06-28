
#include "sat_planning.h"

#include "cadical.hpp"
#include "sat_formula.h"

#include "../search/evaluator.h"

State find_better_state(State state, TaskProxy task_proxy, Evaluator * heuristic, AbstractTask &abstract_task) {
    CaDiCaL::Solver * solver = new CaDiCaL::Solver;
    int T;
    State new_state = state;

    for (T = 0; T < 1000; T++) {
        Formula formula = build_ehc_formula(new_state, task_proxy, T, heuristic);

        for (Clause clause : formula) {
            for (int lit : clause) {
                solver->add(lit);
            }
            solver->add(0);
        }

        new_state = extract_state(solver, task_proxy, T, abstract_task);
        int result = solver->solve();
        
        // The new state is the goal state
        if (is_goal(new_state, task_proxy)) {
            return new_state;
        }

        // UNKNOWN
        if (result == 0) {
            // return error
        }
        // SAT
        else if (result == 10) {
            // We return the new state if it has a better h than the previous state
            if (h(new_state, task_proxy) < h(state, task_proxy)) {
                return new_state;
            }
            break;
        }
        // UNSAT
        else if (result == 20) {
            // There are no further states that can be found, so we return the previous state
            return state;
        }
    }
    return state;
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
int h(State state, TaskProxy task) {
    GoalsProxy goal_state = task.get_goals();
    int count = 0;

    for (FactProxy fact : goal_state) {
        if (state[fact.get_variable()] != fact) {
            count++;
        }
    }

    return count;
}

State extract_state(CaDiCaL::Solver * solver, TaskProxy task_proxy, int T, const AbstractTask &abstract_task) {
    std::vector<int> values(task_proxy.get_variables().size());

    for (VariableProxy var : task_proxy.get_variables()) {
        int lit = lit_encoding(var, T, T);

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