#pragma once

#include <concepts>
#include <iterator>
#include <ranges>
#include "../concepts.hpp"

namespace etch::graph {

namespace node {
// NodeType is node when it is equality_comparable
template <typename NodeType>
concept node_type = std::equality_comparable<NodeType>;

// NodeType has arrival cost when it satisfies all that
// 1. the return value of NodeType::arrival_cost() are totally ordered
// 2. and it is addable
template <typename NodeType>
concept arrival_cost = requires(NodeType n) {
  { n.arrival_cost() } -> std::totally_ordered;
  { n.arrival_cost() } -> addable;
};
}  // namespace node

// GraphType has nodes when typename GraphType::node_type is node type
template <typename GraphType>
concept with_nodes = node::node_type<typename GraphType::node_type>;

// GraphType has iterable nodes iterable when it satisfies all that
// 1. the graph has nodes
// 2. the return type of GraphType::nodes() is forward iterable
// 3. its items are of GraphType::node_type
template <typename GraphType>
concept nodes_iterable = requires(GraphType g) {
  requires with_nodes<GraphType>;
  { g.nodes() } -> std::ranges::forward_range;
  requires std::same_as<std::ranges::range_value_t<decltype(g.nodes())>,
                        typename GraphType::node_type>;
};

namespace edge {
// EdgeType is edge type when
// 1. the return type of EdgeType::from() is node type
// 2. it is same as return type of EdgeType::to()
template <typename EdgeType>
concept edge_type = requires(EdgeType edge) {
  { edge.from() } -> node::node_type;
  requires std::same_as<decltype(edge.from()), decltype(edge.to())>;
};
// EdgeType has weight when it satisfies all that
// 1. the return value of EdgeType::weight() are totally ordered
// 2. and it is addable
template <typename EdgeType>
concept weighted = requires(EdgeType e) {
  { e.weight() } -> std::totally_ordered;
  { e.weight() } -> addable;
};
}  // namespace edge

// GraphType has edges when GraphType::edge_type is edge type
template <typename GraphType>
concept with_edges = edge::edge_type<typename GraphType::edge_type>;

// GraphType has iterable edges when it satisfies all that
// 1. GraphType has edges
// 2. the return type of GraphType::edges() is forward iterable
// 3. its items are of GraphType::edge_type
template <typename GraphType>
concept edges_iterable = requires(GraphType g) {
  requires with_edges<GraphType>;
  { g.edges() } -> std::ranges::forward_range;
  requires std::same_as<std::ranges::range_value_t<decltype(g.edges())>,
                        typename GraphType::edge_type>;
};

// The graph has weighted edges when GraphType::edge_type satisfies weighted_edge
template <typename GraphType>
concept edges_weighted = edge::weighted<typename GraphType::edge_type>;

// NeighborType is neighbor of GraphType when it satisfies either that
// - GraphType does not have weighted edges and NeighborType is of
// GraphType::node_type
// - NeighborType is of std::pair<GraphType::node_type, GraphType::edge_type>
template <typename NeighborType, typename GraphType>
concept neighbor_type_of =
    (!edges_weighted<GraphType> &&
     std::same_as<NeighborType, typename GraphType::node_type>) ||
    std::same_as<NeighborType,
                 std::pair<typename GraphType::node_type,
                           typename GraphType::edge_type>>;

// GraphType has node neighbors when it satisfies all that
// 1. the return type of GraphType::neighbors(const GraphType::node_type&)
// is forward iterable
// 2. its value is neighbor of GraphType
template <typename GraphType>
concept node_neighbors_accessible =
    requires(GraphType g, typename GraphType::node_type n) {
      { g.neighbors(n) } -> std::ranges::forward_range;
      requires neighbor_type_of<
          std::ranges::range_value_t<decltype(g.neighbors(n))>, GraphType>;
    };

// GraphType is graph type when it satisfies all that
// 1. it has nodes
// 2. it has iterable nodes and edges, or it has node neighbors
template <typename GraphType>
concept graph_type =
    with_nodes<GraphType> &&
    ((nodes_iterable<GraphType> && edges_iterable<GraphType>) ||
     node_neighbors_accessible<GraphType>);

// GraphType is weighted graph when it satisfies all that
// 1. it is graph
// 2. its nodes have arrival cost or it has weighted edges
// 3. if both of node arrival cost and edge weight are defined, the return types
// are the same
template <typename GraphType>
concept weighted_graph_type = requires {
    graph_type<GraphType> &&
    (node::arrival_cost<typename GraphType::node_type> ||
     edges_weighted<GraphType>) &&
    !(node::arrival_cost<typename GraphType::node_type> &&
      edges_weighted<GraphType> &&
      !std::same_as<
          std::invoke_result_t<decltype(&GraphType::edge_type::weight),
                               typename GraphType::edge_type*>,
          std::invoke_result_t<decltype(&GraphType::node_type::arrival_cost),
                               typename GraphType::node_type*>>);
};  // namespace etch::graph
