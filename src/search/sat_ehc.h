#ifndef SEARCH_ALGORITHMS_SAT_EHC_H
#define SEARCH_ALGORITHMS_SAT_EHC_H

#include "search_algorithm.h"
#include "../bsc-thesis/sat_planning.h"

namespace sat_ehc {

class SatEhcSearch : public SearchAlgorithm {

    std::shared_ptr<PruningMethod> pruning_method;
    State current_state;
    State initial_state;
    Plan plan;
    better_state result;

protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;

public:
    explicit SatEhcSearch(
        OperatorCost cost_type, int bound, double max_time,
        const std::string &description, utils::Verbosity verbosity);

    virtual void print_statistics() const override;
};
}

#endif