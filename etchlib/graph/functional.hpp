#pragma once

#include <type_traits>
#include "./concepts.hpp"
#include "./type_traits.hpp"

namespace etch::graph {
namespace node {
template <node::arrival_costed NodeType>
requires {n.arrival_cost()}
constexpr inline static auto arrival_cost(const NodeType& n) {
  return n.arrival_cost();
}
}  // namespace node

namespace edge {
template <edge::weighted EdgeType>
constexpr inline static auto weight(const EdgeType& e) {
  return e.weight();
}
}  // namespace edge

template <graph_type GraphType>
struct neighbor_weight {
  auto operator()(const neighbor_type_t<GraphType>& the_neighbor) const {
    if constexpr (std::is_same_v<neighbor_type_t<GraphType>,
                                 typename GraphType::node_type>) {
      if constexpr (node::arrival_cost<typename GraphType::node_type>) {
        return arrival_cost(the_neighbor);
      } else {
        return 1;
      }
    } else if constexpr (std::is_same_v<
                             neighbor_type_t<GraphType>,
                             std::pair<typename GraphType::node_type,
                                       typename GraphType::edge_type>>) {
      return weight(the_neighbor.second);
    } else {
      static_assert(false);
    }
  }
};

}  // namespace etch::graph
