#pragma once

#include <concepts>
#include <iterator>
#include <ranges>
#include "../concepts.hpp"

namespace etch::graph {

namespace node {
// The node of a graph has arrival cost when it satisfies all that
template <typename NodeType>
concept has_arrival_cost = requires(const NodeType& n) {
  // - the return value of NodeType::arrival_cost() are totally ordered and
  // addable
  { n.arrival_cost() + n.arrival_cost() } -> std::totally_ordered;
};
}  // namespace node

namespace edge {
// The edge of a graph is weighted when it satisfies all that
template <typename EdgeType>
concept has_weight = requires(const EdgeType& e) {
  // - the return value of EdgeType::weight() are totally ordered and addable
  { e.weight() + e.weight() } -> std::totally_ordered;
};

}  // namespace edge

// The graph has nodes when
template <typename GraphType>
concept has_nodes = requires {
  // - it defines type GraphType::node_type
  typename GraphType::node_type;
  // - the nodes are copy_constructible
  requires std::copy_constructible<typename GraphType::node_type>;
  // - the nodes are equality_comparable
  requires std::equality_comparable<typename GraphType::node_type>;
};

// The graph has edges when
template <typename GraphType>
concept has_edges = requires {
  // - it defines type GraphType::edge_type
  typename GraphType::edge_type;
};

// The graph has weighted edges when its edge has_weight
template <typename GraphType>
concept has_weighted_edge = edge::has_weight<typename GraphType::edge_type>;

// The nodes of a graph is iterable when
template <typename GraphType>
concept nodes_iterable = requires(const GraphType& g) {
  // - the return type of GraphType::nodes() is forward iterable
  { g.nodes() } -> std::ranges::forward_range;
  // - its items are of node_type
  {
    *std::ranges::begin(g.nodes())
  } -> std::same_as<typename GraphType::node_type>;
};

// The edge of a graph is iterable when
template <typename GraphType>
concept edges_iterable = requires(const GraphType& g) {
  // - the return type of GraphType::edges() is forward iterable
  { g.edges() } -> std::ranges::forward_range;
  // - its items are of edge_type
  {
    *std::ranges::begin(g.edges())
  } -> std::same_as<typename GraphType::edge_type>;
};

// The neighbors of a graph is accessable when
template <typename GraphType>
concept node_neighbor_accessable =
    // - the return type of GraphType::neighbors() is forward iterable
    requires(const GraphType& g) {
      {
        g.neighbors(typename GraphType::node_type())
      } -> std::ranges::forward_range;
    } &&
    //  - its items are of std::pair<node_type, edge_type>, or
    (
        requires(const GraphType& g) {
          {
            std::ranges::range_value_t<decltype(g.neighbors(typename GraphType::node_type()))>()
          } -> std::same_as<std::pair<typename GraphType::node_type,
                                      typename GraphType::edge_type>>;
        } ||
        //   its items are of node_type and it does not have weighted edges
        requires(const GraphType& g) {
          requires !has_weighted_edge<GraphType>;
          {
            std::ranges::range_value_t<decltype(g.neighbors(typename GraphType::node_type()))>()
          } -> std::same_as<typename GraphType::node_type>;
        });

// The graph is valid when
template <typename GraphType>
concept is_graph =
    // it has nodes, and
    has_nodes<GraphType> &&
        // it satisfies any of 1. both nodes and edges are iterable;
        (nodes_iterable<GraphType> && edges_iterable<GraphType>) ||
    // 2. neighbors of nodes are iterable
    node_neighbor_accessable<GraphType>;

// The graph is weighted when
template <typename GraphType>
concept is_weighted_graph =
    // it is graph and
    is_graph<GraphType> &&
    // its nodes have arrival cost or
    (node::has_arrival_cost<typename GraphType::node_type> ||
     // its edges have weight,
     has_weighted_edge<GraphType>) &&
    // and if they are both defined, the return types are the same
    !(node::has_arrival_cost<typename GraphType::node_type> &&
      has_weighted_edge<GraphType> &&
      !std::is_same_v<
          std::invoke_result_t<decltype (&GraphType::edge_type::weight)()>,
          std::invoke_result_t<decltype (
              &GraphType::node_type::arrival_cost)()>>);
};  // namespace etch::graph
