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

namespace upl
{

inline namespace v0_2
{

namespace trait
{

template <class T>
struct element
{
    static_assert(sizeof(T) == -1,
                  "element is not defined for the T");
};

template <class T>
struct ownership
{
    static_assert(sizeof(T) == -1,
                  "ownership is not defined for the T");
};

template <class T>
struct multiplicity
{
    static_assert(sizeof(T) == -1,
                  "multiplicity is not defined for the T");
};

template <class T>
using element_t = typename element<T>::type;

template <class T>
using ownership_t = typename ownership<T>::type;

template <class T>
using multiplicity_t = typename multiplicity<T>::type;

} // namespace trait

} // namespace v0_2

} // namespace upl
