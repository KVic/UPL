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

#include <upl/v0_1/detail/std_smart/bind.h>

namespace upl
{

inline namespace v0_1
{

namespace std_smart
{

template <class T,
          class Ownership = tag::unified,
          class Multiplicity = tag::optional>
using pointer = detail::std_smart::bind::pointer_t<T, Ownership, Multiplicity>;

template <class T, class Multiplicity = tag::optional>
using weak = pointer<T, tag::weak, Multiplicity>;

template <class T, class Multiplicity = tag::optional>
using unified = pointer<T, tag::unified, Multiplicity>;

template <class T, class Multiplicity = tag::optional>
using unique = pointer<T, tag::unique, Multiplicity>;

template <class T, class Multiplicity = tag::optional>
using shared = pointer<T, tag::shared, Multiplicity>;

template <class T>
using weak_optional = weak<T, tag::optional>;

template <class T>
using unified_optional = unified<T, tag::optional>;

template <class T>
using unique_optional = unique<T, tag::optional>;

template <class T>
using shared_optional = shared<T, tag::optional>;

template <class T>
using weak_single = weak<T, tag::single>;

template <class T>
using unified_single = unified<T, tag::single>;

template <class T>
using unique_single = unique<T, tag::single>;

template <class T>
using shared_single = shared<T, tag::single>;

} // namespace std_smart

} // namespace v0_1

} // namespace upl
