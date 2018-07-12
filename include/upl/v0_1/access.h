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

#include <upl/v0_1/concept.h>

#include <utility>

namespace upl
{
inline namespace v0_1
{
namespace
{
template <class P, UPL_CONCEPT_REQUIRES_(StrongPointer<std::decay_t<P>>)>
inline
const P& access(const P& p)
{
    return p;
}

template <class P, UPL_CONCEPT_REQUIRES_(WeakPointer<std::decay_t<P>>)>
inline
auto access(P&& p)
{
    return p.lock();
}

template <class P, class SuccessAction>
inline
auto access(P&& p, SuccessAction success_action)
{
    const auto& accessor = access(std::forward<P>(p));
    if (accessor)
        return success_action(*accessor);
}

template <class P, class SuccessAction, class FailureAction>
inline
auto access(P&& p,
            SuccessAction success_action,
            FailureAction failure_action)
{
    const auto& accessor = access(std::forward<P>(p));
    if (accessor)
        return success_action(*accessor);
    else
        return failure_action();
}
} // namespace
} // namespace v0_1
} // namespace upl
