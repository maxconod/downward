#include "sat_ehc.h"

#include "heuristics/goal_count_heuristic.h"

#include "../bsc-thesis/sat_planning.h"

#include "./plugins/plugin.h"
#include "./search_algorithms/search_common.h"
#include "./task_utils/task_properties.h"
#include "./tasks/root_task.h"

using namespace std;

namespace sat_ehc {

bool is_same_state(State a, State b) {
    a.unpack();
    b.unpack();
    return a.get_unpacked_values() == b.get_unpacked_values();
}

SatEhcSearch::SatEhcSearch(
    GoalFormula goal_formula, OperatorCost cost_type, int bound, double max_time,
    const string &description, utils::Verbosity verbosity)
    : SearchAlgorithm(cost_type, bound, max_time, description, verbosity),
      goal_formula(goal_formula),
      current_state(state_registry.get_initial_state()),
      initial_state(state_registry.get_initial_state()){
}

void SatEhcSearch::print_statistics() const {
    statistics.print_detailed_statistics();
    search_space.print_statistics();
    // pruning_method->print_statistics();
}

void SatEhcSearch::initialize() {
    log << "init of SAT-based EHC search" << endl;

    shared_ptr<Evaluator> goal_count_h = make_shared<goal_count_heuristic::GoalCountHeuristic>(task, false, "goalcount", utils::Verbosity::SILENT);
    shared_ptr<Evaluator> ff_h = make_shared<goal_count_heuristic::GoalCountHeuristic>(task, false, "ff", utils::Verbosity::SILENT);

    initial_state = state_registry.get_initial_state();
}

// We consider 5 configs
SearchStatus SatEhcSearch::step() {

    switch (goal_formula) {
    case GoalFormula::AG:
        return step_ag();
    case GoalFormula::GC:
        return step_gc();
    case GoalFormula::SAT:
        return step_sat();
    }

    log << "didn't find any existing goal_formula" << endl;
    return FAILED;
}

// Searching as SAT_EHC with better_formula
SearchStatus SatEhcSearch::step_ag() {
    current_state = initial_state;

    log << "reached AG" << endl;

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

// Searching as SAT_EHC using the plugged-in goal-count heuristic
SearchStatus SatEhcSearch::step_gc() {
    current_state = initial_state;
    log << "reached GC" << endl;

    if (check_goal_and_set_plan(current_state)) {
        return SOLVED;
    }

    while (h_goal_count(current_state, task_proxy) > 0) {

        better_state result = find_better_state(current_state, task_proxy, goal_count_h.get(), *task, statistics);

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

    if (task_properties::is_goal_state(task_proxy, current_state)) {
        log << "Solution found!" << endl;
        set_plan(plan);
        return SOLVED;
    }

    return FAILED;
}

// Searching as SAT_EHC with the original sat planning
SearchStatus SatEhcSearch::step_sat() {
    log << "reached SAT" << endl;

    Model model = solve_formula(initial_state, task_proxy);

    if (model.result == 10) {
        set_plan(model.plan);
        return SOLVED;
    }
    return FAILED;
}

SearchStatus SatEhcSearch::expand(const SearchNode &node) {
    statistics.inc_expanded();

    const State &state = node.get_state();
    if (check_goal_and_set_plan(state)) {
        return SOLVED;
    }

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
        add_option<GoalFormula>("goal_formula", "list of search algorithms");
    }

    virtual shared_ptr<SatEhcSearch> create_component(
        const plugins::Options &opts) const override {
        return plugins::make_shared_from_arg_tuples<SatEhcSearch>(
        opts.get<GoalFormula>("goal_formula"),
        get_search_algorithm_arguments_from_options(opts));
    }
};

static plugins::FeaturePlugin<SatEhcSearchFeature> _plugin;
static plugins::TypedEnumPlugin<GoalFormula> _enum_plugin(
    {{"AG", "Searching as SAT_EHC with better_formula"},
    {"GC", "Searching as SAT_EHC using the plugged-in goal-count heuristic"},
    {"SAT", "Searching as SAT_EHC with the original sat planning"}
});
}