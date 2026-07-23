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
      current_state(state_registry.get_initial_state()),
      initial_state(state_registry.get_initial_state()) {
}

void SatEhcSearch::print_statistics() const {
    statistics.print_detailed_statistics();
    search_space.print_statistics();
    // pruning_method->print_statistics();
}

void SatEhcSearch::initialize() {
    log << "init of SAT-based EHC search" << endl;

    // initial_state = state_registry.get_initial_state();
}

SearchStatus SatEhcSearch::step() {
    current_state = initial_state;

    if (check_goal_and_set_plan(current_state)) {
        return SOLVED;
    }

    while (h_goal_count(current_state, task_proxy) > 0) {
        log << "it loops through the while-loop of sat_ehc with h=" << h_goal_count(current_state, task_proxy) << endl;

        better_state result = find_better_state(current_state, task_proxy, nullptr, *task, statistics);

        if (is_same_state(result.state, current_state)) {
            log << "the state wasn't improved, so failed" << endl;
            statistics.inc_dead_ends();
            return FAILED;
        }

        statistics.inc_expanded();
        current_state = result.state;
        int plan_size = result.plan.size();

        for (int i = 0; i < plan_size; i++) {
            plan.push_back(result.plan.at(i));
        }
    }
    
    log << "after the while-loop of sat_ehc containing h=" << h_goal_count(current_state, task_proxy) << endl;

    //if (check_goal_and_set_plan(current_state)) {
   //     return SOLVED;
    //}

    if (task_properties::is_goal_state(task_proxy, current_state)) {
        log << "Solution found!" << endl;
        set_plan(plan);
        return SOLVED;
    }

    return FAILED;
}

SearchStatus SatEhcSearch::expand(const SearchNode &node) {
    statistics.inc_expanded();

    const State &state = node.get_state();
    if (check_goal_and_set_plan(state))
        return SOLVED;

    // generate_successors(node);
    return IN_PROGRESS;
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