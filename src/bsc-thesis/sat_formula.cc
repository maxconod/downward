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

Formula transition_formula(Formula formula, OperatorProxy operator_proxy, int T, int t) {
    PreconditionsProxy pre = operator_proxy.get_preconditions();
    EffectsProxy effects_proxy = operator_proxy.get_effects();
    Clause clause = {};

    for (FactProxy fact_proxy : pre) {
        VariableProxy var = fact_proxy.get_variable();
        int lit = lit_encoding(var, T, t);

        // If the variable is false, then we negate it
        if (fact_proxy.get_value() == 1) {
            lit = -lit;
        }

        clause.push_back({lit});
    }

    formula.push_back(clause);

    for (EffectProxy eff : effects_proxy) {
        FactProxy fact = eff.get_fact();
        VariableProxy var = fact.get_variable();
        int lit = lit_encoding(var, T, t);

        // If the variable is false, then we negate it
        if (fact_proxy.get_value() == 1) {
            lit = -lit;
        }

        clause.push_back({lit});
    }

    formula.push_back(clause);
    return formula;
}

Formula goal_formula(Formula formula, TaskProxy task_proxy, int T) {
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
