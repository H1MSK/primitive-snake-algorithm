#include <ranges>
#include "../graph/path/a_star.hpp"

template<etch::graph::edges_weighted Graph>
void test(Graph graph) {
}

#include <array>
class SimpleRing {
public:
  struct edge {
    int p;
    int from() const {return p; }
    int to() const {return (p + 1) % 10; }
    int weight() const { return 1; }
  };

  using node_type = int;
  using edge_type = edge;

  void test() {
    ::test(*this);
  }
  std::ranges::iota_view<int, int> nodes() const { return std::views::iota(0, 10); }

  auto edges() const {
    return std::views::iota(0, 10) |
           std::views::transform([](int x) { return edge(x); });
  }

  auto neighbors(int x) const {
    return std::array<int, 2>{ (x + 9) % 10, (x + 1) % 10 };
  }
  auto dis(int a, int b) {
    return etch::graph::path::AStar<SimpleRing>(*this, a, b,
      [](int a, int b) { return abs(a - b); });
  }
};

int main() {
}