#include <concepts>

template <typename T>
concept c = requires(T x) {
  { x + 1 } -> std::same_as<int>;
};

enum { A, B } myEnum;
void foo() {}

class Foo {
  void f() { foo(); }
};
void f() {
  foo();
}
void f(int a, int b, int c) {
  if (a)
    return;

  if (b)
    return;
  else
    return;

  if (c)
    return;
  else {
    return;
  }

  auto lambda = [](int a) {};
  auto lambda2 = [](int a) { return a; };
  auto lambda = [](int x, int y) { return x < y; };

  sort(a.begin(), a.end(), [](int x, int y) { return x < y; });
  while (true)
    continue;
}