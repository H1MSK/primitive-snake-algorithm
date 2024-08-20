#pragma once

#include <type_traits>

namespace etch
{

template<typename Type>
using function_pass_t = std::conditional_t<std::is_scalar_v<Type>, Type, const Type&>;

} // namespace etch
