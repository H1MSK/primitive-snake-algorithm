#pragma once

namespace etch
{
// Numerics
template<typename T>
concept addable = requires(const T a, const T b) {
    a + b;
};

} // namespace etch
