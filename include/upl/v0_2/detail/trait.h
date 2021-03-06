/*
 * MIT License
 *
 * Copyright (c) 2018-2019 Viktor Kireev
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

#include <upl/v0_2/trait.h>

#include <upl/v0_2/detail/internal/pointer.h>

namespace upl
{

inline namespace v0_2
{

namespace trait
{

template <class T, class Multiplicity>
struct element<upl::detail::weak<T, Multiplicity>>
{ using type = typename upl::detail::weak<T, Multiplicity>::element_type; };

template <class T, class Multiplicity>
struct element<upl::detail::unified<T, Multiplicity>>
{ using type = typename upl::detail::unified<T, Multiplicity>::element_type; };

template <class T, class Multiplicity>
struct element<upl::detail::unique<T, Multiplicity>>
{ using type = typename upl::detail::unique<T, Multiplicity>::element_type; };

template <class T, class Multiplicity>
struct element<upl::detail::shared<T, Multiplicity>>
{ using type = typename upl::detail::shared<T, Multiplicity>::element_type; };

template <class T, class Multiplicity>
struct ownership<upl::detail::weak<T, Multiplicity>>
{ using type = tag::weak; };

template <class T, class Multiplicity>
struct ownership<upl::detail::unified<T, Multiplicity>>
{ using type = tag::unified; };

template <class T, class Multiplicity>
struct ownership<upl::detail::unique<T, Multiplicity>>
{ using type = tag::unique; };

template <class T, class Multiplicity>
struct ownership<upl::detail::shared<T, Multiplicity>>
{ using type = tag::shared; };

template <class T, class Multiplicity_>
struct multiplicity<upl::detail::weak<T, Multiplicity_>>
{ using type = Multiplicity_; };

template <class T, class Multiplicity_>
struct multiplicity<upl::detail::unified<T, Multiplicity_>>
{ using type = Multiplicity_; };

template <class T, class Multiplicity_>
struct multiplicity<upl::detail::unique<T, Multiplicity_>>
{ using type = Multiplicity_; };

template <class T, class Multiplicity_>
struct multiplicity<upl::detail::shared<T, Multiplicity_>>
{ using type = Multiplicity_; };

} // namespace trait

} // inline namespace v0_2

} // namespace upl
