
#include "sat_planning.h"

#include "cadical.hpp"
#include "sat_formula.h"

#include "../search/evaluator.h"

State find_better_state(State state, TaskProxy task_proxy, Evaluator * heuristic) {
    CaDiCaL::Solver * solver = new CaDiCaL::Solver;
    int T;

    for (T = 0; T < 1000; T++) {
        Formula formula = build_ehc_formula(state, task_proxy, T, heuristic);

        for (Clause clause : formula) {
            for (int lit : clause) {
                solver->add(lit);
            }
            solver->add(0);
        }

        int result = solver->solve();

        // UNKNOWN
        if (result == 0) {
            // return error
        }
        // SAT
        else if (result == 10) {
            new_state = extract_state(solver, task_proxy, T);
            break;
        }
        // UNSAT
        else if (result == 20) {
            // There are no further states that can be found, so we return the previous state
            return state;
        }
    }
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

State extract_state(CaDiCaL::Solver * solver, TaskProxy task_proxy, int T) {
    // Temporal initial state that will be
    State state = task_proxy.get_initial_state();

    for (VariableProxy var : task_proxy.get_variables()) {
        int lit = lit_encoding(var, T, T);

        if (solver->val(lit) > 0) {
            state[var.get_id()] = 0;
        }
        else {
            state[var.get_id()] = 1;
        }
    }

    return state;
}