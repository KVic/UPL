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

#include <upl/v0_2/tag.h>
#include <upl/v0_2/trait.h>
#include <upl/v0_2/detail/internal/utility/concept.h>

#include <type_traits>

namespace upl
{

inline namespace v0_2
{

namespace internal
{

namespace
{

template <class P, class T>
UPL_CONCEPT_SPECIFIER PointerOfElement =
    std::is_same_v<T, void>
    || std::is_same_v<T, trait::element_t<P>>
    || std::is_base_of_v<T, trait::element_t<P>>;

template <class P>
UPL_CONCEPT_SPECIFIER BasePointer =
    std::is_base_of_v<upl::internal::tag::owner_based, trait::ownership_t<P>>;

template <class P, class Ownership>
UPL_CONCEPT_SPECIFIER OwnershipPointer =
    std::is_base_of_v<Ownership, trait::ownership_t<P>>;

template <class P, class Multiplicity>
UPL_CONCEPT_SPECIFIER MultiplicityPointer =
    std::is_base_of_v<Multiplicity, trait::multiplicity_t<P>>;

} // namespace

} // namespace internal

namespace
{

template <class P, class Ownership, class Multiplicity>
UPL_CONCEPT_SPECIFIER BasePointer =
    (  (  std::is_same_v<Ownership, upl::tag::any>
       && internal::BasePointer<P>)
    || internal::OwnershipPointer<P, Ownership>)
    && (  std::is_same_v<Multiplicity, upl::tag::any>
       || internal::MultiplicityPointer<P, Multiplicity>);

template <class P, class T = void>
UPL_CONCEPT_SPECIFIER Pointer =
    internal::BasePointer<P>
    && internal::PointerOfElement<P, T>;

template <class P, class T = void>
UPL_CONCEPT_SPECIFIER StrongPointer =
    internal::OwnershipPointer<P, internal::tag::strong>
    && internal::PointerOfElement<P, T>;

template <class P, class T = void>
UPL_CONCEPT_SPECIFIER WeakPointer =
    internal::OwnershipPointer<P, internal::tag::weak>
    && internal::PointerOfElement<P, T>;

template <class P, class T = void>
UPL_CONCEPT_SPECIFIER UnifiedPointer =
    internal::OwnershipPointer<P, tag::unified>
    && internal::PointerOfElement<P, T>;

template <class P, class T = void>
UPL_CONCEPT_SPECIFIER StrictPointer =
    internal::OwnershipPointer<P, internal::tag::strict>
    && internal::PointerOfElement<P, T>;

template <class P, class T = void>
UPL_CONCEPT_SPECIFIER UniquePointer =
    internal::OwnershipPointer<P, tag::unique>
    && internal::PointerOfElement<P, T>;

template <class P, class T = void>
UPL_CONCEPT_SPECIFIER SharedPointer =
    internal::OwnershipPointer<P, tag::shared>
    && internal::PointerOfElement<P, T>;

template <class P, class T = void>
UPL_CONCEPT_SPECIFIER OptionalPointer =
    internal::BasePointer<P>
    && internal::MultiplicityPointer<P, tag::optional>
    && internal::PointerOfElement<P, T>;

template <class P, class T = void>
UPL_CONCEPT_SPECIFIER SinglePointer =
    internal::BasePointer<P>
    && internal::MultiplicityPointer<P, tag::single>
    && internal::PointerOfElement<P, T>;

} // namespace

} // namespace v0_2

} // namespace upl
