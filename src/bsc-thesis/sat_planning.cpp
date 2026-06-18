
#include "sat_planning.h"
#include "cadical.hpp"
#include "sat_formula.h"

Model solve_formula(TaskProxy task_proxy) {
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
    }

    if (result == 10) {
        model = {task_proxy, T, result};
    }

    return model;
}
