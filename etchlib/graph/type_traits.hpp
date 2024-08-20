#pragma once

#include "../type_traits.hpp"
#include "./concepts.hpp"

namespace etch::graph {

template <graph_type GraphType>
using node_type_t = GraphType::node_type;
template <graph_type GraphType>
using node_type_fpt = function_pass_t<node_type_t<GraphType>>;

template <graph_type GraphType>
using edge_type_t = GraphType::edge_type;
template <graph_type GraphType>
using node_type_fpt = function_pass_t<node_type_t<GraphType>>;

template <weighted_graph_type GraphType, bool = edges_weighted<GraphType>>
struct path_weight_type {
  using type = std::invoke_result_t<decltype(&edge_type_t<GraphType>::weight),
                                    edge_type_t<GraphType>*>;
};

template <weighted_graph_type GraphType>
struct path_weight_type<GraphType, false> {
  using type =
      std::invoke_result_t<decltype(&node_type_t<GraphType>::arrival_cost),
                           node_type_t<GraphType>*>;
};

template <weighted_graph_type GraphType>
using path_weight_type_t = path_weight_type<GraphType>::type;

template <node_neighbors_accessible GraphType>
using neighbor_type_t = std::ranges::range_value_t<
    std::invoke_result_t<decltype(&GraphType::neighbors),
                         GraphType*,
                         typename GraphType::node_type>>;
}  // namespace etch::graph
