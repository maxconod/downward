
#include "sat_planning.h"
#include "cadical.hpp"
#include "sat_formula.h"

std::optional<std::vector<int>> solve_formula(TaskProxy task_proxy) {
    CaDiCaL::Solver * solver = new CaDiCaL::Solver;

    for (int T = 0; T < 1000; T++) {
        Formula formula = build_sat_formula(task_proxy, T);

        // Using the dimacs format
        for (Clause clause : formula) {
            for (int lit : clause) {
                solver->add(lit);
            }
            solver->add(0);
        }

        int result = solver->solve();

        // UNKNOWN
        if (result == 0) {
            return std::nullopt;
        }
        // SATISFIABLE
        else if (result == 10) {
            break;
        }
    }

    std::vector<int> model;
    return model;
}
