#pragma once
#include <ranges>
#include "../type_traits.hpp"

namespace etch::graph::path
{

template <typename DistanceType>
concept distance_type = requires(DistanceType a, DistanceType b) {
  requires std::totally_ordered<DistanceType>;
  {a + b} -> std::same_as<DistanceType>;
};

template <typename Container, typename GraphType>
concept path_container_type = requires(Container c) {
  requires std::ranges::range<Container>;
  requires std::same_as<std::ranges::range_value_t<Container>,
                        node_type_t<GraphType>>;
  c.push_back(std::ranges::range_value_t<Container>());
};

template <typename Container, typename GraphType>
concept visited_node_container_type = requires(Container c) {
  requires std::ranges::range<Container>;
  requires std::same_as<std::ranges::range_value_t<Container>,
                        node_type_t<GraphType>>;
  c.insert(std::ranges::range_value_t<Container>());
  {
    c.contains(std::ranges::range_value_t<Container>())
  } -> std::convertible_to<bool>;
};


// Calculator is node distance calculator when it satisfies all that
// - it is invocable as Calculator(GraphType::node_type, GraphType::node_type)
// - the result value can be add with `path_weight_type_t`, and the result is
// totally ordered
//
// Note: the result has not to be of `path_weight_type_t`
template <typename Calculator, typename GraphType>
concept distance_calculator_compatible_to = requires {
  requires std::invocable<Calculator, typename GraphType::node_type,
                          typename GraphType::node_type>;
  {
    std::invoke_result_t<Calculator, typename GraphType::node_type,
                         typename GraphType::node_type>() +
        path_weight_type_t<GraphType>()
  } -> std::totally_ordered;
};

} // namespace etch::graph::path
