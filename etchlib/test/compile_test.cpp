#include <ranges>
#include "../graph/AStar.hpp"

template<etch::graph::nodes_iterable Graph>
void test(Graph graph) {}

#include <array>
class SimpleRing {
public:
  using node_type = int;
  using edge_type = std::pair<int, int>;

  void test() {
    auto t = std::views::iota(0, 10);
    auto &tt = t;
    auto l = std::ranges::begin(tt);
    auto r = std::ranges::end(tt);
    ::test(*this);
  }
  std::ranges::iota_view<int, int> nodes() const { return std::views::iota(0, 10); }

  auto edges() const {
    return std::views::iota(0, 10) |
           std::views::transform([](int x) { return std::pair(x, (x + 1) % 10); });
  }

  auto neighbors(int x) const {
    return std::array<int, 2>{ (x + 9) % 10, (x + 1) % 10 };
  }
  auto dis(int a, int b) {
    return etch::graph::path_finding::a_star::AStar<SimpleRing>(*this, a, b);
  }
};
