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

#include <upl/v0_2/detail/internal/pointer.h>

namespace upl
{

inline namespace v0_2
{

namespace detail
{

namespace bind
{

template <class T, class Ownership, class Multiplicity>
struct pointer
{
    static_assert(sizeof(T) == -1,
                  "pointer is not defined for the T");
};

template <class T, class Multiplicity>
struct pointer<T, tag::weak, Multiplicity>
{ using type = weak<T, Multiplicity>; };

template <class T, class Multiplicity>
struct pointer<T, tag::unified, Multiplicity>
{ using type = unified<T, Multiplicity>; };

template <class T, class Multiplicity>
struct pointer<T, tag::unique, Multiplicity>
{ using type = unique<T, Multiplicity>; };

template <class T, class Multiplicity>
struct pointer<T, tag::shared, Multiplicity>
{ using type = shared<T, Multiplicity>; };

template <class T, class Ownership, class Multiplicity>
using pointer_t = typename pointer<T, Ownership, Multiplicity>::type;

} // namespace bind

} // namespace detail

} // namespace v0_2

} // namespace upl
