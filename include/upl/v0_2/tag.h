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

namespace upl
{

inline namespace v0_2
{

namespace internal
{

namespace tag
{

struct owner_based {};
struct strong : public owner_based {};
struct weak : public owner_based {};
struct strict : public strong {};

} // namespace tag

} // namespace internal

namespace tag
{

struct any {};

using weak = internal::tag::weak;
struct unified : public internal::tag::strong {};
struct unique : public internal::tag::strict {};
struct shared : public internal::tag::strict {};

struct optional {};
struct single {};

} // namespace tag

} // namespace v0_2

} // namespace upl
