#pragma once

namespace etch
{

namespace internal
{
template<bool Condition, typename T_true, typename>
struct type_select_struct {
    using type = T_true;
};

template<typename T_true, typename T_false>
struct type_select_struct<false, T_true, T_false> {
    using type = T_false;
};
} // namespace internal

template<bool Condition, typename T_true, typename T_false>
using type_select = internal::type_select_struct<Condition, T_true, T_false>;

} // namespace etch
