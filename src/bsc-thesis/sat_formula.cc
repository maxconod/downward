#include "sat_formula.h"

Formula build_sat_formula(TaskProxy task, int T) {

    Formula formula = Formula();
    VariablesProxy vars = task.get_variables();
    OperatorsProxy ops = task.get_operators();

    if (T <= 0) {
        throw std::invalid_argument("Negative timestamp");
    }

    formula = initial_formula(formula, task);

    for (int t=1; t<T; t++) {
        formula = transition_formula(formula, task, t);
    }

    formula = goal_formula(formula, task);
}
    
Formula initial_formula(Formula formula, TaskProxy task) {
    State initial_state = task.get_initial_state();
    initial_state.unpack();

    for (FactProxy fact : initial_state) {
        FactPair pair = fact.get_pair();
        int value = fact.get_value();
        int lit = 0;
        if (value == 0) {
            lit = 
        }
        else {
            lit =
        }
        formula.push_back({lit});
    }

    return formula;
}


Formula transition_formula(Formula formula, TaskProxy task, int T) {
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



Formula goal_formula(Formula formula, TaskProxy task) {
    State goal_state = task.get_goals();
    goal_state.unpack();

    for (FactProxy fact : goal_state) {
        FactPair pair = fact.get_pair();
        FactPair value = fact.get_value();

        int lit = 0;
        if (value == 0) {
            lit =
        }
        else {
            lit =
        }

        formula.push_back({lit});
    }

    return formula;
}
