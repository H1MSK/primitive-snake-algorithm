#pragma once

#include "./concepts.hpp"

namespace etch::graph::path {

template <typename GraphType,
          distance_calculator_compatible_to<GraphType> Calculator>
using distance_type_t = std::
    invoke_result_t<Calculator, node_type_t<GraphType>, node_type_t<GraphType>>;

}  // namespace etch::graph::path
