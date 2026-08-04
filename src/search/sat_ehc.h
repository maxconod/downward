#ifndef SEARCH_ALGORITHMS_SAT_EHC_H
#define SEARCH_ALGORITHMS_SAT_EHC_H

#include "search_algorithm.h"
#include "../bsc-thesis/sat_planning.h"

namespace sat_ehc {

enum class GoalFormula {
    AC,
    BF,
    SAT
};

class SatEhcSearch : public SearchAlgorithm {

    GoalFormula goal_formula;
    std::shared_ptr<PruningMethod> pruning_method;
    State current_state;
    State initial_state;
    Plan plan;
    std::shared_ptr<Evaluator> goal_count_h;
    std::shared_ptr<Evaluator> ff_h;
    //better_state result;

    SearchStatus expand(const SearchNode &node);
    SearchStatus step_ac();
    SearchStatus step_bf();
    SearchStatus step_sat();

protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;

public:
    explicit SatEhcSearch(GoalFormula goal_formula,
        OperatorCost cost_type, int bound, double max_time,
        const std::string &description, utils::Verbosity verbosity);

    virtual void print_statistics() const override;
};
}

#endif