#pragma once

#include <list>
#include <set>
#include <type_traits>
#include "./concepts.hpp"
#include "./type_traits.hpp"

namespace etch::graph::path {

namespace a_star {

template <typename State, typename GraphType, typename Predictor>
concept is_state = requires(State s) {
  requires distance_calculator_compatible_to<Predictor, GraphType>;
  State(node_type_t<GraphType>(), path_weight_type_t<GraphType>(),
        distance_type_t<GraphType, Predictor>());
  { s.node() } -> std::same_as<node_type_t<GraphType>>;
  { s.penalty() } -> std::totally_ordered;
};

template <typename Container, typename GraphType, typename Predictor>
concept state_container =
    std::ranges::range<Container> &&
    is_state<std::ranges::range_value_t<Container>, GraphType, Predictor>;

template <graph_type Graph,
          distance_calculator_compatible_to<Graph> predict_distance_type,
          typename step_type = path_weight_type_t<Graph>,
          std::totally_ordered penalty_type = path_weight_type_t<Graph>>
struct State {
  node_type_t<Graph> _node;
  path_weight_type_t<Graph> _penalty;
  State(node_type_t<Graph>& node,
        const path_weight_type_t<Graph>& step,
        const path_weight_type_t<Graph>& predict_distance)
      : _node(node), _penalty(step + predict_distance) {}
  node_type_t<Graph>& node() const { return _node; }
  path_weight_type_t<Graph>& penalty() const { return _penalty; }
};
}  // namespace a_star

template <
    node_neighbors_accessible Graph,
    distance_calculator_compatible_to<Graph> NodeDistancePredictor,
    path_container_type<Graph> PathContainer =
        std::list<node_type_t<Graph>>,
    visited_node_container_type<Graph> VisitedNodeContainer =
        std::set<node_type_t<Graph>>,
    a_star::state_container<Graph, NodeDistancePredictor> StateContainer =
        std::set<a_star::State<Graph,
                               distance_type_t<Graph, NodeDistancePredictor>>>>
PathContainer AStar(const Graph& graph,
                    function_pass_t<node_type_t<Graph>> from,
                    function_pass_t<node_type_t<Graph>> to,
                    NodeDistancePredictor predictor) {
  VisitedNodeContainer visit;
  StateContainer states;
  visit.insert(from);
  states.insert(StateType(from, path_weight_type_t<Graph>()));
  while (states.begin() != states.end()) {
    auto s = states.begin();
    states.erase(s);
  }
};
}  // namespace etch::graph::path
