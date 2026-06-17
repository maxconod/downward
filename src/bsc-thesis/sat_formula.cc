#include "sat_formula.h"

Formula build_sat_formula(TaskProxy task, int T) {

    Formula formula = Formula();

    if (T <= 0) {
        throw std::invalid_argument("Negative timestamp");
    }

    formula = initial_formula(formula, task, T);

    for (int t=0; t<T; t++) {
        formula = transition_formula(formula, task, T, t);
    }

    formula = goal_formula(formula, task, T);

    return formula;
}
    
Formula initial_formula(Formula formula, TaskProxy task_proxy, int T) {
    // State => [(v0,0), (v1,1), ...]
    State initial_state = task_proxy.get_initial_state();
    Clause clause = {};
    //initial_state.unpack();

    // Extracting variables+value from initial state
    for (FactProxy fact_proxy : initial_state) {
        VariableProxy var = fact_proxy.get_variable();
        int lit = lit_encoding(var, T, 0);

        // If the variable is false, then we negate it
        if (fact_proxy.get_value() == 1) {
            lit = -lit;
        }

        clause.push_back({lit});
    }

    formula.push_back(clause);
    return formula;
}

Formula transition_formula(Formula formula, TaskProxy task, int T, int t) {


    for (size_t i = 0; i < ops.size(); i++) {
        for (FactProxy pre : ops[i].get_preconditions()) {
            FactPair pair = pre.get_pair();
            int lit =
            formula.push_back({lit});
        }
        for (EffectProxy eff : ops[i].get_effects()) {
            FactPair pair = eff.get_fact().get_pair();
            int lit =
            formula.push_back({lit});
        }
    }

    return formula;
}

Formula goal_formula(Formula formula, TaskProxy task, int T) {
    GoalsProxy goal_state = task_proxy.get_goals();
    Clause clause = {};

    // Extracting variables+value from goal state
    for (FactProxy fact_proxy : goal_state) {
        VariableProxy var = fact_proxy.get_variable();
        int lit = lit_encoding(var, T, T);

        // If the variable is false, then we negate it
        if (fact_proxy.get_value() == 1) {
            lit = -lit;
        }

        clause.push_back({lit});
    }

    formula.push_back(clause);
    return formula;
}

int lit_encoding(VariableProxy var, int T, int t) {
    return var.get_id() * (T+1) + t + 1;
}
