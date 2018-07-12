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

#include "StdSmart.h"

namespace upl
{
inline namespace v0_1
{
namespace detail
{
namespace std_smart
{
template <class T, class Multiplicity>
inline void swap(shared<T, Multiplicity>& a,
                 shared<T, Multiplicity>& b) noexcept (internal::IsOptional<Multiplicity>)
{ a.swap(b); }

template <class T, class Multiplicity>
inline void swap(unique<T, Multiplicity>& a,
                 unique<T, Multiplicity>& b) noexcept (internal::IsOptional<Multiplicity>)
{ a.swap(b); }

template <class T, class Multiplicity>
inline void swap(unified<T, Multiplicity>& a,
                 unified<T, Multiplicity>& b) noexcept (internal::IsOptional<Multiplicity>)
{ a.swap(b); }

template <class T, class Multiplicity>
inline void swap(weak<T, Multiplicity>& a,
                 weak<T, Multiplicity>& b) noexcept (internal::IsOptional<Multiplicity>)
{ a.swap(b); }
} // namespace std_smart
} // namespace detail
} // namespace v0_1
} // namespace upl
