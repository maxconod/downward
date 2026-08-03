#include "sat_formula.h"
#include <unordered_set>
#include "../search/evaluator.h"
#include "../search/utils/logging.h"

Formula build_sat_formula(TaskProxy task, int T) {

    Formula formula = Formula();

    if (T < 0) {
        throw std::invalid_argument("Negative timestamp");
    }

    formula = initial_formula(formula, task, 0);

    for (int t=0; t<T; t++) {
        formula = transition_formula(formula, task, T, t);
    }

    formula = goal_formula(formula, task, T);

    return formula;
}

Formula build_ehc_formula(State state, TaskProxy task, int T, Evaluator * h) {
    Formula formula = Formula();

    if (T < 0) {
        throw std::invalid_argument("Negative timestamp");
    }

    formula = actual_state_formula(formula, state, task, 0);

    for (int t=0; t<T; t++) {
        formula = transition_formula(formula, task, T, t);
    }

    formula = better_formula(formula, state, task, T);

    return formula;
}

Formula initial_formula(Formula formula, TaskProxy task_proxy, int T) {
    // State => [(v0,1), (v1,0), (v2,1), ...]
    State initial_state = task_proxy.get_initial_state();
    Clause clause = {};
    //initial_state.unpack();

    // Extracting variables+value from initial state
    for (FactProxy fact_proxy : initial_state) {
        VariableProxy var = fact_proxy.get_variable();
        int lit = lit_encoding(var, task_proxy, 0);

        // If the variable is false, then we negate it
        if (fact_proxy.get_value() == 1) {
            lit = -lit;
        }

        // One clause per variable
        formula.push_back({lit});
    }
    clause.clear();
    return formula;
}

Formula transition_formula(Formula formula, TaskProxy task_proxy, int T, int t) {
    OperatorsProxy operators = task_proxy.get_operators();
    VariablesProxy vars = task_proxy.get_variables();
    Clause clause = {};
    // Adding all operators in one clause
    Clause operators_clause = {};

    for (OperatorProxy op : operators) {

        int lit_op = lit_encoding_op(op, task_proxy, t);

        // Only choosing one operator per transition
        // e.g. (negA1 OR negA2)
        for (size_t j = op.get_id() + 1; j < operators.size(); j++) {
            OperatorProxy op2 = task_proxy.get_operators()[j];

            int lit_op2 = lit_encoding_op(op2, task_proxy, t);

            clause.push_back(-lit_op);
            clause.push_back(-lit_op2);
            formula.push_back(clause);
            clause.clear();
        }


        PreconditionsProxy pre = op.get_preconditions();
        EffectsProxy effects_proxy = op.get_effects();
        std::unordered_set<int> eff_var_ids;

        // Precondition
        for (FactProxy fact_proxy : pre) {
            // Add operator
            clause.push_back(-lit_op);

            VariableProxy var = fact_proxy.get_variable();
            int lit = lit_encoding(var, task_proxy, t);

            // If the variable is false, then we negate it
            if (fact_proxy.get_value() == 1) {
                lit = -lit;
            }

            clause.push_back(lit);
            formula.push_back(clause);
            clause.clear();
        }

        // Effect
        for (EffectProxy eff : effects_proxy) {
            // Add biimplication clauses for variables that are not effects
            FactProxy fact = eff.get_fact();
            VariableProxy var = fact.get_variable();
            eff_var_ids.insert(var.get_id());
            // Add operator
            clause.push_back(-lit_op);

            int lit = lit_encoding(var, task_proxy, t+1);

            // If the variable is false, then we negate it
            if (fact.get_value() == 1) {
                lit = -lit;
            }

            clause.push_back(lit);
            formula.push_back(clause);
            clause.clear();
        }


        operators_clause.push_back(lit_op);


        // Add the biimplication (A -> (v0 <-> v1))
        // which equals = (negA OR (neg_v0 OR v1)) AND (negA OR (neg_v1 OR v0)))
        for (VariableProxy var : vars) {
            if (eff_var_ids.find(var.get_id()) == eff_var_ids.end()) {
                // int lit_op = lit_encoding_op(op, task_proxy, t);
                int lit_v0  = lit_encoding(var, task_proxy, t);
                int lit_v1  = lit_encoding(var, task_proxy, t+1);
                clause.push_back(-lit_op);
                clause.push_back(-lit_v0);
                clause.push_back(lit_v1);
                formula.push_back(clause);
                clause.clear();

                clause.push_back(-lit_op);
                clause.push_back(-lit_v1);
                clause.push_back(lit_v0);
                formula.push_back(clause);
                clause.clear();
            }
        }
    }
    formula.push_back(operators_clause);


    // Making clauses only using operators: Make sure only one operator can be chosen by the sat solver
    int count = 0;
    for (OperatorProxy op : operators) {
        count++;
        int lit_op = lit_encoding_op(op, task_proxy, t);
        for (std::size_t j = count; j < operators.size(); j++) {
            clause.push_back(-lit_op);

            OperatorProxy op2 = operators[j];
            int lit_op2 = lit_encoding_op(op2, task_proxy, t);
            clause.push_back(-lit_op2);
            formula.push_back(clause);
            clause.clear();
        }
    }

    return formula;
}

Formula goal_formula(Formula formula, TaskProxy task_proxy, int T) {
    GoalsProxy goal_state = task_proxy.get_goals();
    Clause clause = {};

    // Extracting variables+value from goal state
    for (FactProxy fact_proxy : goal_state) {
        VariableProxy var = fact_proxy.get_variable();
        int lit = lit_encoding(var, task_proxy, T);

        // If the variable is false, then we negate it
        if (fact_proxy.get_value() == 1) {
            lit = -lit;
        }

        formula.push_back({lit});
    }

    clause.clear();
    return formula;
}

Formula better_formula(Formula formula, const State &current_state, TaskProxy task_proxy, int T) {
    std::vector<FactProxy> goal_facts;

    int satisfied = 0;
    for (FactProxy goal : task_proxy.get_goals()) {
        if (current_state[goal.get_variable()] == goal) {
            satisfied++;
        }
        goal_facts.push_back(goal);
    }

    int R = satisfied + 1;
    formula = add_at_least_r(formula, task_proxy, goal_facts, R, T);

    utils::g_log << "goes through the better formula after claling method" << std::endl;
    return formula;
}

Formula actual_state_formula(Formula formula, State current_state, TaskProxy task_proxy, int T) {
    // Clause clause = {};

    // Extracting variables+value from initial state
    for (FactProxy fact_proxy : current_state) {
        VariableProxy var = fact_proxy.get_variable();
        int lit = lit_encoding(var, task_proxy, 0);

        // If the variable is false, then we negate it
        if (fact_proxy.get_value() == 1) {
            lit = -lit;
        }

        formula.push_back({lit});
    }

    return formula;
}

// Took inspiration from the paper: A comparison of encodings for cardinality constraints in a SAT solver from Ed Wynn
Formula add_at_least_r(Formula formula, TaskProxy task_proxy, const std::vector<FactProxy> &goal_facts, int R, int T) {
    int n = goal_facts.size();
    utils::g_log << "r=" << R << ", n=" << n << std::endl;

    std::vector<int> x_lits(n);
    for (int i = 0; i < n; i++) {
        FactProxy goal = goal_facts[i];
        int lit = lit_encoding(goal.get_variable(), task_proxy, T);

        if (goal.get_value() == 1) {
            lit = -lit;
        }

        x_lits[i] = -lit;
    }
    utils::g_log << "testing-----------" << std::endl;


    R = n - R;
    std::vector<std::vector<int>> e(R+1, std::vector<int>(n));
    Clause clause = {};

    // if R=0, then all variables are false.
    if (R <= 0) {
        for (int lit : x_lits) {
            formula.push_back({-lit});
        }
        return formula;
    }

    // Assigning a number to each variable e
    int m = task_proxy.get_variables().size() + task_proxy.get_operators().size();
    int offset = T * m * 2;

    // Encode each variable e to a unique value
    int count = 1;
    for (int k = 1; k <= R; k++) {
        for (int j = k; j <= n+k-R-1; j++) {
            e[k][j] = offset + count;
            count++;
        }
    }

    // Adding equation 2
    for (int k = 1; k <= R; k++) {
        for (int j = k; j <= n+k-R-2; j++) {
            clause.push_back(-e[k][j]);
            clause.push_back(e[k][j+1]);
            formula.push_back(clause);
            clause.clear();
        }
    }

    // Adding equation 3
    for (int k = 1; k <= R-1; k++) {
        for (int j = k; j <= n+k-R-1; j++) {
            clause.push_back(-e[k][j]);
            clause.push_back(e[k+1][j+1]);
            clause.push_back(-x_lits[j]);
            formula.push_back(clause);
            clause.clear();
        }
    }

    // e[0][j] is implicitly true
    for (int j = 0; j <= n-R-1; j++) {
        clause.push_back(e[1][j+1]);
        clause.push_back(-x_lits[j]);
        formula.push_back(clause);
        clause.clear();
    }

    // e[r+1][j] is implicitly false
    for (int j = R; j <= n-1; j++) {
        clause.push_back(-e[R][j]);
        clause.push_back(-x_lits[j]);
        formula.push_back(clause);
        clause.clear();
    }

    return formula;

}

int lit_encoding(VariableProxy var, TaskProxy task_proxy, int t) {
    //return var.get_id() * (T+1) + t + 1;
    int n = task_proxy.get_variables().size() + task_proxy.get_operators().size();
    return t * n + var.get_id() + 1;
}

int lit_encoding_op(OperatorProxy op, TaskProxy task_proxy, int t) {
    int n = task_proxy.get_variables().size() + task_proxy.get_operators().size();
    return t * n + task_proxy.get_variables().size() + op.get_id() + 1;
}
