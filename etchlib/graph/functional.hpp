#pragma once

#include <type_traits>
#include "concepts.hpp"

namespace etch::graph {

template <is_graph GraphType>
struct defaultWeightToNeighbor {
  auto operator()(
      const GraphType::node_type& from,
      const std::ranges::range_value_t<decltype(g.neighbors(typename GraphType::node_type()))>& the_neightbor) const {
    if constexpr (std::is_same_v<std::ranges::range_value_t<decltype(g.neighbors(typename GraphType::node_type()))>,
                                 typename GraphType::node_type>) {
      if constexpr (node::has_arrival_cost<typename GraphType::node_type>) {
        return the_neightbor.arrival_cost();
      } else {
        return 1;
      }
    } else if constexpr (has_weighted_edge<GraphType> &&
                         std::ranges::range_value_t<decltype(g.neighbors(typename GraphType::node_type()))>,
                             std::pair<typename GraphType::node_type,
                                       typename GraphType::edge_type>>) {
      return the_neightbor.second.weight();
    } else {
      static_assert(false);
    }
  }
};

}  // namespace etch::graph
