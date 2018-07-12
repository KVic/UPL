/*
 * MIT License
 *
 * Copyright (c) 2018 Viktor Kireev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "Counted.h"

#include <functional>

namespace upl
{
inline namespace v0_1
{
namespace detail
{
namespace counted
{
namespace internal
{
template <class T, class TM, class U, class UM>
inline bool operator==(const strong<T, TM>& a,
                       const strong<U, UM>& b) noexcept
{ return a.get() == b.get(); }

template <class T, class TM, class U, class UM>
inline bool operator!=(const strong<T, TM>& a,
                       const strong<U, UM>& b) noexcept
{ return !(a == b); }

template <class T, class TM, class U, class UM>
inline bool operator<(const strong<T, TM>& a,
                      const strong<U, UM>& b) noexcept
{
    using E_T = typename strong<T, TM>::element_type;
    using E_U = typename strong<U, UM>::element_type;
    using V   = typename std::common_type_t<E_T*, E_U*>;
    return std::less<V>()(a.get(), b.get());
}

template <class T, class TM, class U, class UM>
inline bool operator>(const strong<T, TM>& a,
                      const strong<U, UM>& b) noexcept
{ return b < a; }

template <class T, class TM, class U, class UM>
inline bool operator<=(const strong<T, TM>& a,
                       const strong<U, UM>& b) noexcept
{ return !(b < a); }

template <class T, class TM, class U, class UM>
inline bool operator>=(const strong<T, TM>& a,
                       const strong<U, UM>& b) noexcept
{ return !(a < b); }

template <class T, class TM>
inline bool
operator==(const strong<T, TM>& a, std::nullptr_t) noexcept
{ return !a; }

template <class T, class TM>
inline bool
operator==(std::nullptr_t, const strong<T, TM>& a) noexcept
{ return !a; }

template <class T, class TM>
inline bool
operator!=(const strong<T, TM>& a, std::nullptr_t) noexcept
{ return static_cast<bool>(a); }

template <class T, class TM>
inline bool
operator!=(std::nullptr_t, const strong<T, TM>& a) noexcept
{ return static_cast<bool>(a); }

template <class T, class TM>
inline bool
operator<(const strong<T, TM>& a, std::nullptr_t) noexcept
{
    using E_T = typename strong<T, TM>::element_type;
    return std::less<E_T*>()(a.get(), nullptr);
}

template <class T, class TM>
inline bool
operator<(std::nullptr_t, const strong<T, TM>& a) noexcept
{
    using E_T = typename strong<T, TM>::element_type;
    return std::less<E_T*>()(nullptr, a.get());
}

template <class T, class TM>
inline bool
operator>(const strong<T, TM>& a, std::nullptr_t) noexcept
{ return nullptr < a; }

template <class T, class TM>
inline bool
operator>(std::nullptr_t, const strong<T, TM>& a) noexcept
{ return a < nullptr; }

template <class T, class TM>
inline bool
operator<=(const strong<T, TM>& a, std::nullptr_t) noexcept
{ return !(nullptr < a); }

template <class T, class TM>
inline bool
operator<=(std::nullptr_t, const strong<T, TM>& a) noexcept
{ return !(a < nullptr); }

template <class T, class TM>
inline bool
operator>=(const strong<T, TM>& a, std::nullptr_t) noexcept
{ return !(a < nullptr); }

template <class T, class TM>
inline bool
operator>=(std::nullptr_t, const strong<T, TM>& a) noexcept
{ return !(nullptr < a); }
} // namespace internal
} // namespace counted
} // namespace detail
} // namespace v0_1
} // namespace upl
