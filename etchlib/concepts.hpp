#pragma once

#include <ranges>

namespace etch
{
// Numerics

template<typename T>
concept addable = requires(const T a, const T b) {
    {a + b} -> std::same_as<T>;
};

// Ranges
template<typename R>
concept insertable_range = requires(R r) {
    requires std::ranges::range<R>;
    r.insert(std::ranges::range_value_t<R>());
};
template<typename R>
concept erasable_range = requires(R r) {
    requires std::ranges::range<R>;
    r.erase(std::ranges::iterator_t<R>());
};

} // namespace etch
