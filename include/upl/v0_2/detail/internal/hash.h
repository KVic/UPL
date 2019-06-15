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

#include "pointer.h"

namespace std
{

template <class T, class Multiplicity>
struct hash<upl::detail::internal::strong<T, Multiplicity>>
{
    using pointer_type = upl::detail::internal::strong<T, Multiplicity>;
    using element_type = typename pointer_type::element_type;

    using argument_type = pointer_type;
    using result_type   = typename hash<element_type*>::result_type;

    result_type operator()(const pointer_type& pointer) const noexcept
    {
        return hash<element_type*>()(pointer.get());
    }
};

template <class T, class Multiplicity>
struct hash<upl::detail::unified<T, Multiplicity>>
    : public hash<upl::detail::internal::strong<T, Multiplicity>> {};

template <class T, class Multiplicity>
struct hash<upl::detail::unique<T, Multiplicity>>
    : public hash<upl::detail::internal::strong<T, Multiplicity>> {};

template <class T, class Multiplicity>
struct hash<upl::detail::shared<T, Multiplicity>>
    : public hash<upl::detail::internal::strong<T, Multiplicity>> {};

} // namespace std
