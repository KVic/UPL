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

#include <upl/v0_1/bind.h>
#include <upl/v0_1/detail/counted.h>

namespace upl
{
inline namespace v0_1
{
namespace bind
{
template <class T, class Multiplicity>
struct pointer<T, tag::weak, Multiplicity>
{ using type = upl::detail::counted::weak<T, Multiplicity>; };

template <class T, class Multiplicity>
struct pointer<T, tag::unified, Multiplicity>
{ using type = upl::detail::counted::unified<T, Multiplicity>; };

template <class T, class Multiplicity>
struct pointer<T, tag::unique, Multiplicity>
{ using type = upl::detail::counted::unique<T, Multiplicity>; };

template <class T, class Multiplicity>
struct pointer<T, tag::shared, Multiplicity>
{ using type = upl::detail::counted::shared<T, Multiplicity>; };
} // namespace bind
} // namespace v0_1
} // namespace upl
