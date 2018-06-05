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

#include <upl/v0_1/tag.h>
#include <upl/v0_1/trait.h>

#include <type_traits>

namespace upl
{
inline namespace v0_1
{
namespace internal
{
namespace
{
template <class P, class Ownership>
inline constexpr bool OwnershipPointer =
    std::is_base_of_v<Ownership, trait::ownership_t<P>>;

template <class P, class Multiplicity>
inline constexpr bool MultiplicityPointer =
    std::is_base_of_v<Multiplicity, trait::multiplicity_t<P>>;

template <class P, class Ownership, class Multiplicity>
inline constexpr bool TaggedPointer =
    (  std::is_same_v<Ownership, upl::tag::any>
    || OwnershipPointer<P, Ownership>)
    && (  std::is_same_v<Multiplicity, upl::tag::any>
       || MultiplicityPointer<P, Multiplicity>);
} // namespace
} // namespace internal

namespace
{
template <class P>
inline constexpr bool Pointer =
    internal::OwnershipPointer<P, internal::tag::owner_based>;

template <class P>
inline constexpr bool StrongPointer =
    internal::OwnershipPointer<P, internal::tag::strong>;

template <class P>
inline constexpr bool WeakPointer =
    internal::OwnershipPointer<P, internal::tag::weak>;

template <class P>
inline constexpr bool UnifiedPointer =
    internal::OwnershipPointer<P, tag::unified>;

template <class P>
inline constexpr bool StrictPointer =
    internal::OwnershipPointer<P, internal::tag::strict>;

template <class P>
inline constexpr bool UniquePointer =
    internal::OwnershipPointer<P, tag::unique>;

template <class P>
inline constexpr bool SharedPointer =
    internal::OwnershipPointer<P, tag::shared>;

template <class P>
inline constexpr bool OptionalPointer =
    internal::MultiplicityPointer<P, tag::optional>;

template <class P>
inline constexpr bool SinglePointer =
    internal::MultiplicityPointer<P, tag::single>;
} // namespace
} // namespace v0_1
} // namespace upl
