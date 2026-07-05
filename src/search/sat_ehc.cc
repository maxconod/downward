#include "sat_ehc.h"

#include "./search_algorithms/search_common.h"

#include "./plugins/plugin.h"
#include "./task_utils/task_properties.h"
#include "./tasks/root_task.h"
#include "../bsc-thesis/sat_planning.h"

using namespace std;

namespace sat_ehc {

bool is_same_state(State a, State b) {
    a.unpack();
    b.unpack();
    return a.get_unpacked_values() == b.get_unpacked_values();
}

SatEhcSearch::SatEhcSearch(
    OperatorCost cost_type, int bound, double max_time,
    const string &description, utils::Verbosity verbosity)
    : SearchAlgorithm(cost_type, bound, max_time, description, verbosity),
      current_state(state_registry.get_initial_state()) {
}

void SatEhcSearch::print_statistics() const {
    statistics.print_detailed_statistics();
    search_space.print_statistics();
    pruning_method->print_statistics();
}

void SatEhcSearch::initialize() {
    log << "init of SAT-based EHC search" << endl;

    State initial_state = state_registry.get_initial_state();
}

SearchStatus SatEhcSearch::step() {
    current_state = initial_state;

    if (check_goal_and_set_plan(current_state)) {
        return SOLVED;
    }

    if (h_goal_count(current_state, task_proxy) > 0) {
        better_state result = find_better_state(current_state, task_proxy, nullptr, *task);

        if (is_same_state(result.state, current_state)) {
            log << "the state wasn't improved, so failed" << endl;
            return FAILED;
        }

        current_state = result.state;
        int plan_size = result.plan.size();

        for (int i = 0; i < plan_size; i++) {
            plan.push_back(result.plan.at(i));
        }
    }

    if (check_goal_and_set_plan(current_state)) {
        return SOLVED;
    }

    return IN_PROGRESS;
}

void SatEhcSearch::print_statistics() const {
    log << "Plan length: " << plan.size() << endl;
}

class SatEhcSearchFeature
    : public plugins::TypedFeature<SearchAlgorithm, SatEhcSearch> {
public:
    SatEhcSearchFeature() : TypedFeature("sat_ehc") {
        document_title("SAT-EHC search");
        document_synopsis("Great descripion");

        add_search_algorithm_options_to_feature(*this, "sat_ehc");
    }

    virtual shared_ptr<sat_ehc::SatEhcSearch> create_component(
        const plugins::Options &opts) const override {
        return plugins::make_shared_from_arg_tuples<sat_ehc::SatEhcSearch>(
            get_search_algorithm_arguments_from_options(opts));
    }
};

static plugins::FeaturePlugin<SatEhcSearchFeature> _plugin;
}