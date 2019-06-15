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

#include <upl/v0_2/concept.h>
#include <upl/v0_2/detail/concrete.h>

namespace upl
{

inline namespace v0_2
{

namespace conform
{

template <class T>
using weak_t = upl::weak<trait::element_t<T>, trait::multiplicity_t<T>>;

template <class P, UPL_CONCEPT_REQUIRES_(upl::Pointer<std::decay_t<P>>)>
inline
auto weak(P&& pointer)
{
    return weak_t<std::decay_t<P>>{std::forward<P>(pointer)};
}

} // namespace conform

} // namespace v0_2

} // namespace upl
