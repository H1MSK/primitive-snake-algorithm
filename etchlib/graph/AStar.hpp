#include <list>
#include <ranges>
#include <set>
#include <type_traits>
#include "./concepts.hpp"
#include "./functional.hpp"

namespace etch::graph::path_finding::a_star {

template <typename Graph>
class AStarState {
  typename Graph::node_type node;
};

template <typename Graph,
          std::ranges::forward_range PathContainer =
              std::list<typename Graph::node_type>,
          typename DistanceCalculator = defaultWeightToNeighbor<Graph>>
  requires(node_neighbor_accessable<Graph> &&
           std::invocable<DistanceCalculator,
                          typename Graph::node_type,
                          typename Graph::node_type>)
PathContainer AStar(const Graph& graph,
                    const typename Graph::node_type& from,
                    const typename Graph::node_type& to) {
  using visit_container = std::set<typename Graph::node_type>;
  visit_container visit;
};
}  // namespace etch::graph::path_finding::a_star
