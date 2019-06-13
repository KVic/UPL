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
#include <upl/v0_1/exception.h>
#include <upl/v0_1/utility/itself.h>

#include "Utility/Concept.h"

#include <memory>
#include <cassert>

namespace upl
{

inline namespace v0_1
{

namespace detail
{

namespace std_smart
{

namespace internal
{

namespace
{

template <class T, class Y>
inline constexpr bool IsCompatible = std::is_convertible_v<Y*, T*>;

template <class T, class Y>
inline constexpr bool IsIncompatible = !IsCompatible<std::remove_const_t<T>,
                                                     std::remove_const_t<Y>>;

// Const incorrectness conversion: 'const Y* -> T*'.
template <class T, class Y>
inline constexpr bool IsConstIncorrect =
    std::is_const_v<Y>
    && !std::is_const_v<T>
    && IsCompatible<std::remove_const_t<T>,
                    std::remove_const_t<Y>>;

template <class Multiplicity>
static constexpr bool IsOptional =
    std::is_same_v<Multiplicity, tag::optional>;

template <class Multiplicity>
static constexpr bool IsSingle =
    std::is_same_v<Multiplicity, tag::single>;

} // namespace

template <template <class Y, class M> class StdSmart, class Y, class M>
inline void utilize(StdSmart<Y, M>&& other) noexcept
{ StdSmart<Y, tag::optional>{std::move(other)}; }

template <class T, class Multiplicity>
class base
{
protected:
    using multiplicity_type = Multiplicity;

    static constexpr bool IsOptional = internal::IsOptional<Multiplicity>;
    static constexpr bool IsSingle   = internal::IsSingle<Multiplicity>;

public:
    using element_type = std::remove_extent_t<T>;

protected:
    UPL_CONCEPT_REQUIRES(IsSingle)
    void handle_empty_single_access() const
    { throw single_error{"'single' is empty"}; }

    UPL_CONCEPT_REQUIRES(IsSingle)
    void handle_empty_single_swap() const
    { throw single_error{"'single' can't be swapped with a null pointer"}; }
};

template <class T, class Multiplicity>
class weak;

template <class T, class Multiplicity>
class strong : public base<T, Multiplicity>
{
protected:
    using parent = base<T, Multiplicity>;
    using typename parent::multiplicity_type;

    template <class Y, class D>
    using UniqueReferrer = std::unique_ptr<Y, D>;
    template <class Y>
    using SharedReferrer = std::shared_ptr<Y>;
    template <class Y>
    using WeakReferrer = std::weak_ptr<Y>;

    using Referrer = SharedReferrer<T>;

public:
    using typename base<T, multiplicity_type>::element_type;

    // Default constructors.
    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    constexpr strong() noexcept : m_referrer{} {}

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    strong() = delete;

    // Copy constructors.
    template <class Y, class D>
    strong(const UniqueReferrer<Y, D>& referrer) = delete;

    // Move constructors.
    template <class Y, class D, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    strong(UniqueReferrer<Y, D>&& referrer) noexcept (parent::IsOptional)
        : m_referrer{std::move(referrer)}
    {
        // TODO: Optimize.
        if constexpr (parent::IsSingle)
            if (!m_referrer)
                throw single_error{"'single' can't be moved "
                                   "from a null pointer"};
    }

    template <class Y, class D, UPL_CONCEPT_REQUIRES_(IsIncompatible<T, Y>)>
    strong(UniqueReferrer<Y, D>&& referrer) = delete;

    template <class Y, class D, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<T, Y>)>
    strong(UniqueReferrer<Y, D>&& referrer) = delete;

    // Copy operators.
    template <class Y, class D>
    strong& operator=(const UniqueReferrer<Y, D>& referrer) = delete;

    // Move operators.
    template <class Y, class D, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    strong& operator=(UniqueReferrer<Y, D>&& referrer) noexcept (parent::IsOptional)
    {
        strong{std::move(referrer)}.swap_nothrow(*this);
        return *this;
    }

    template <class Y, class D, UPL_CONCEPT_REQUIRES_(IsIncompatible<T, Y>)>
    strong& operator=(UniqueReferrer<Y, D>&& referrer) = delete;

    template <class Y, class D, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<T, Y>)>
    strong& operator=(UniqueReferrer<Y, D>&& referrer) = delete;

    template <class U, class M>
    bool owner_before(const strong<U, M>& other) const noexcept (parent::IsOptional && strong<U, M>::IsOptional)
    {
        // TODO: Should this throw an exception?
        if constexpr (parent::IsSingle)
            this->check_empty_single_access();

        return other.owner_before_inverse(*this);
    }

    template <class U, class M>
    bool owner_before(const weak<U, M>& other) const noexcept (parent::IsOptional && weak<U, M>::IsOptional)
    {
        // TODO: Should this throw an exception?
        if constexpr (parent::IsSingle)
            this->check_empty_single_access();

        return other.owner_before_inverse(*this);
    }

    element_type* get() const noexcept (parent::IsOptional)
    {
        // TODO: Should this throw an exception?
        if constexpr (parent::IsSingle)
            this->check_empty_single_access();

        return m_referrer.get();
    }

    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    explicit operator bool() const noexcept
    { return static_cast<bool>(m_referrer); }
    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    explicit constexpr operator bool() const noexcept
    { return true; }

    T& operator*() const noexcept (parent::IsOptional)  { return *get(); }
    T* operator->() const noexcept (parent::IsOptional) { return get(); }

    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    void reset() noexcept { m_referrer.reset(); }

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    void reset() = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    void reset(Y* p)
    {
        assert(this->get() == nullptr || this->get() != p);
        strong{p}.swap_nothrow(*this);
    }

protected:
    // Value constructors.
    template <class Y>
    explicit strong(Y* p) noexcept (parent::IsOptional)
        : m_referrer{p}
    {
        // TODO: Optimize.
        if constexpr (parent::IsSingle)
            if (p == nullptr)
                throw single_error{"'single' can't be created "
                                   "from a null pointer."};
    }

    // Copy constructors.
    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    strong(const SharedReferrer<Y>& referrer) noexcept (parent::IsOptional)
        : m_referrer{referrer}
    {
        // TODO: Optimize.
        if constexpr (parent::IsSingle)
            if (!m_referrer)
                throw single_error{"'single' can't be copied "
                                   "from a null pointer"};
    }

    template <class Y, UPL_CONCEPT_REQUIRES_(IsIncompatible<T, Y>)>
    strong(const SharedReferrer<Y>& referrer) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<T, Y>)>
    strong(const SharedReferrer<Y>& referrer) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    explicit strong(const WeakReferrer<Y>& referrer)
        : m_referrer{referrer} {}

    template <class Y, UPL_CONCEPT_REQUIRES_(IsIncompatible<T, Y>)>
    strong(const WeakReferrer<Y>& referrer) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<T, Y>)>
    strong(const WeakReferrer<Y>& referrer) = delete;

    strong(const strong& other) noexcept (parent::IsOptional)
        : strong{other.m_referrer} {}

    template <class Y, class M>
    strong(const strong<Y, M>& other) noexcept (parent::IsOptional)
        : strong{other.m_referrer} {}

    // Move constructors.
    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    strong(SharedReferrer<Y>&& referrer) noexcept (parent::IsOptional)
        : m_referrer{std::move(referrer)}
    {
        // TODO: Optimize.
        if constexpr (parent::IsSingle)
            if (!m_referrer)
                throw single_error{"'single' can't be moved "
                                   "from a null pointer"};
    }

    template <class Y, UPL_CONCEPT_REQUIRES_(IsIncompatible<T, Y>)>
    strong(SharedReferrer<Y>&& referrer) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<T, Y>)>
    strong(SharedReferrer<Y>&& referrer) = delete;

    strong(strong&& other) noexcept (parent::IsOptional)
        : strong{std::move(other.m_referrer)} {}

    template <class Y, class M>
    strong(strong<Y, M>&& other) noexcept (parent::IsOptional)
        : strong{std::move(other.m_referrer)} {}

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    void swap(strong<Y, multiplicity_type>& other) noexcept (parent::IsOptional)
    {
        if constexpr (parent::IsSingle)
            check_empty_single_swap();

        other.swap_inverse(*this);
    }

    template <class Y>
    void swap_nothrow(strong<Y, multiplicity_type>& other) noexcept
    { m_referrer.swap(other.m_referrer); }

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    void check_empty_single_access() const
    {
        if (!m_referrer)
            this->handle_empty_single_access();
    }

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    void check_empty_single_swap() const
    {
        if (!m_referrer)
            this->handle_empty_single_swap();
    }

    Referrer copy_referrer() const noexcept
    { return Referrer{m_referrer}; }

    Referrer move_referrer() noexcept
    { return Referrer{std::move(m_referrer)}; }

private:
    template <class U, class M>
    bool owner_before_inverse(const strong<U, M>& other) const noexcept (parent::IsOptional)
    {
        // TODO: Should this throw an exception?
        if constexpr (parent::IsSingle)
            this->check_empty_single_access();

        return m_referrer.owner_before(other.m_referrer);
    }

    template <class U, class M>
    bool owner_before_inverse(const weak<U, M>& other) const noexcept (parent::IsOptional)
    {
        // TODO: Should this throw an exception?
        if constexpr (parent::IsSingle)
            this->check_empty_single_access();

        return m_referrer.owner_before(other.m_referrer);
    }

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    void swap_inverse(strong<Y, multiplicity_type>& other) noexcept (parent::IsOptional)
    {
        if constexpr (parent::IsSingle)
            check_empty_single_swap();

        this->swap_nothrow(other);
    }

    template <class Y, class multiplicity_type>
    friend class strong;
    template <class Y, class multiplicity_type>
    friend class weak;

    Referrer m_referrer{};
};

template <class T, class Multiplicity>
class strict : public strong<T, Multiplicity>
{
protected:
    using parent = strong<T, Multiplicity>;
    using typename parent::multiplicity_type;

public:
    // Default constructor.
    strict() = default;

    // Value constructors.
    constexpr strict(std::nullptr_t) noexcept
        : strict{} {}

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    explicit strict(Y* p) : parent{p} {}

    template <class Y, UPL_CONCEPT_REQUIRES_(IsIncompatible<T, Y>)>
    strict(Y* p) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<T, Y>)>
    strict(Y* p) = delete;

    // Itself constructors.
    template <class ... Args, UPL_CONCEPT_REQUIRES_(!std::is_abstract_v<T>)>
    explicit strict(itself_t, Args&& ... args)
        : parent{std::make_shared<T>(std::forward<Args>(args) ...)} {}

    template <class ... Args, UPL_CONCEPT_REQUIRES_(std::is_abstract_v<T>)>
    strict(itself_t, Args&& ... args) = delete;

    template <class Y, class ... Args, UPL_CONCEPT_REQUIRES_(  IsCompatible<T, Y>
                                                            && !std::is_abstract_v<Y>)>
    explicit strict(itself_type_t<Y>, Args&& ... args)
        : parent{std::make_shared<Y>(std::forward<Args>(args) ...)} {}

    template <class Y, class ... Args, UPL_CONCEPT_REQUIRES_(IsIncompatible<T, Y>)>
    strict(itself_type_t<Y>, Args&& ... args) = delete;

    template <class Y, class ... Args, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<T, Y>)>
    strict(itself_type_t<Y>, Args&& ... args) = delete;

    template <class Y, class ... Args, UPL_CONCEPT_REQUIRES_(std::is_abstract_v<Y>)>
    strict(itself_type_t<Y>, Args&& ... args) = delete;

protected:
    using parent::strong;
    using parent::operator=;
};

template <class T, class Multiplicity>
class weak : public base<T, Multiplicity>
{
protected:
    using parent = base<T, Multiplicity>;
    using typename parent::multiplicity_type;

    template <class Y, class D>
    using UniqueReferrer = std::unique_ptr<Y, D>;
    template <class Y>
    using SharedReferrer = std::shared_ptr<Y>;
    template <class Y>
    using WeakReferrer = std::weak_ptr<Y>;

    using Referrer = WeakReferrer<T>;

public:
    // Default constructors.
    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    constexpr weak() noexcept {}

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    weak() = delete;

    // Copy constructors.
    template <class Y, class D>
    weak(const UniqueReferrer<Y, D>& referrer) = delete;

    // Move constructors.
    template <class Y, class D>
    weak(UniqueReferrer<Y, D>&& referrer) = delete;

    template <class Y>
    weak(SharedReferrer<Y>&& referrer) = delete;

    // Copy operators.
    template <class Y, class D>
    weak& operator=(const UniqueReferrer<Y, D>& referrer) = delete;

    // Move operators.
    template <class Y, class D>
    weak& operator=(UniqueReferrer<Y, D>&& referrer) = delete;

    template <class Y>
    weak& operator=(SharedReferrer<Y>&& referrer) = delete;

    template <class U, class M>
    bool owner_before(const weak<U, M>& other) const noexcept (parent::IsOptional && weak<U, M>::IsOptional)
    { return other.owner_before_inverse(*this); }

    template <class U, class M>
    bool owner_before(const strong<U, M>& other) const noexcept (parent::IsOptional && strong<U, M>::IsOptional)
    { return other.owner_before_inverse(*this); }

    bool expired() const noexcept
    { return m_referrer.expired(); }

    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    void reset() noexcept { m_referrer.reset(); }

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    void reset() = delete;

protected:
    // Copy constructors.
    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    weak(const WeakReferrer<Y>& referrer) noexcept
        : m_referrer{referrer} {}

    template <class Y, UPL_CONCEPT_REQUIRES_(IsIncompatible<T, Y>)>
    weak(const WeakReferrer<Y>& referrer) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<T, Y>)>
    weak(const WeakReferrer<Y>& referrer) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    weak(const SharedReferrer<Y>& referrer) noexcept
        : m_referrer{referrer} {}

    template <class Y, UPL_CONCEPT_REQUIRES_(IsIncompatible<T, Y>)>
    weak(const SharedReferrer<Y>& referrer) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<T, Y>)>
    weak(const SharedReferrer<Y>& referrer) = delete;

    weak(const weak& other) noexcept
        : weak{other.m_referrer} {}

    template <class Y, class M>
    weak(const weak<Y, M>& other) noexcept
        : weak{other.m_referrer} {}

    template <class Y, class M>
    explicit weak(const strong<Y, M>& other) noexcept
        : weak{other.m_referrer} {}

    // Move constructors.
    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    weak(WeakReferrer<Y>&& referrer) noexcept
        : m_referrer{std::move(referrer)} {}

    template <class Y, UPL_CONCEPT_REQUIRES_(IsIncompatible<T, Y>)>
    weak(WeakReferrer<Y>&& referrer) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<T, Y>)>
    weak(WeakReferrer<Y>&& referrer) = delete;

    weak(weak&& other) noexcept
        : weak{std::move(other.m_referrer)} {}

    template <class Y, class M>
    weak(weak<Y, M>&& other) noexcept
        : weak{std::move(other.m_referrer)} {}

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    void swap(weak<Y, multiplicity_type>& other) noexcept
    { m_referrer.swap(other.m_referrer); }

    SharedReferrer<T> lock() const noexcept { return m_referrer.lock(); }

private:
    template <class U, class M>
    bool owner_before_inverse(const weak<U, M>& other) const noexcept (parent::IsOptional)
    { return m_referrer.owner_before(other.m_referrer); }

    template <class U, class M>
    bool owner_before_inverse(const strong<U, M>& other) const noexcept (parent::IsOptional)
    { return m_referrer.owner_before(other.m_referrer); }

    template <class Y, class multiplicity_type>
    friend class strong;
    template <class Y, class multiplicity_type>
    friend class weak;

    Referrer m_referrer;
};

} // namespace internal

template <class T, class Multiplicity>
class unified;
template <class T, class Multiplicity>
class unique;
template <class T, class Multiplicity>
class shared;
template <class T, class Multiplicity>
class weak;

template <class T, class Multiplicity>
class unified : public internal::strong<T, Multiplicity>
{
private:
    using parent = internal::strong<T, Multiplicity>;
    using typename parent::multiplicity_type;

    template <class Y>
    static constexpr bool IsCompatible = internal::IsCompatible<T, Y>;

    template <class Y>
    static constexpr bool IsIncompatible = internal::IsIncompatible<T, Y>;

    template <class Y>
    static constexpr bool IsConstIncorrect = internal::IsConstIncorrect<T, Y>;

    template <class Y>
    using SharedReferrer = typename parent::template SharedReferrer<Y>;
    template <class Y>
    using WeakReferrer = typename parent::template WeakReferrer<Y>;

public:
    using parent::strong;
    using parent::operator=;

    // Copy constructors.
    unified(const unified& other) noexcept (parent::IsOptional)
        : parent{other} {}

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unified(const internal::strong<Y, M>& other) noexcept (parent::IsOptional)
        : parent{other} {}

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unified(const weak<Y, M>& other) noexcept (parent::IsOptional)
        : unified{other.lock()} {}

    template <class Y>
    unified(const SharedReferrer<Y>& referrer) noexcept (parent::IsOptional)
        : parent{referrer} {}

    template <class Y>
    unified(const WeakReferrer<Y>& referrer) noexcept (parent::IsOptional)
        : unified{referrer.lock()} {}

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unified(const StdSmart<Y, M>& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unified(const StdSmart<Y, M>& other) = delete;

    // Move constructors.
    unified(unified&& other) = default;

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unified(unified<Y, M>&& other) noexcept (parent::IsOptional)
        : parent{std::move(other)} {}

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && (  std::is_base_of_v<unique<Y, M>, StdSmart<Y, M>>
                                      || std::is_base_of_v<shared<Y, M>, StdSmart<Y, M>>))>
    unified(StdSmart<Y, M>&& other) noexcept (parent::IsOptional)
        : parent{std::move(other)} {}

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<weak<Y, M>, StdSmart<Y, M>>)>
    unified(StdSmart<Y, M>&& other)
        : unified{other.lock()}
    { internal::utilize(std::move(other)); }

    template <class Y>
    unified(SharedReferrer<Y>&& referrer) noexcept (parent::IsOptional)
        : parent{std::move(referrer)} {}

    template <class Y>
    unified(WeakReferrer<Y>&& referrer) noexcept (parent::IsOptional)
        : unified{referrer.lock()}
    { referrer.reset(); }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unified(StdSmart<Y, M>&& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unified(StdSmart<Y, M>&& other) = delete;

    // Copy operators.
    unified& operator=(const unified& other) noexcept
    {
        return this->template operator=<internal::strong, T, multiplicity_type>(
            static_cast<const internal::strong<T, multiplicity_type>&>(other));
    }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<internal::strong<Y, M>,
                                                        StdSmart<Y, M>>)>
    unified& operator=(const StdSmart<Y, M>& other) noexcept (parent::IsOptional)
    {
        unified{other}.swap_nothrow(*this);
        return *this;
    }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<weak<Y, M>, StdSmart<Y, M>>)>
    unified& operator=(const StdSmart<Y, M>& other)
    {
        unified{other}.swap_nothrow(*this);
        return *this;
    }

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unified& operator=(const SharedReferrer<Y>& referrer) noexcept (parent::IsOptional)
    {
        unified{referrer}.swap_nothrow(*this);
        return *this;
    }

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unified& operator=(const WeakReferrer<Y>& referrer) noexcept (parent::IsOptional)
    {
        unified{referrer}.swap_nothrow(*this);
        return *this;
    }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unified& operator=(const StdSmart<Y, M>& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unified& operator=(const StdSmart<Y, M>& other) = delete;

    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    unified& operator=(std::nullptr_t) noexcept
    {
        internal::utilize(std::move(*this));
        return *this;
    }

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    unified& operator=(std::nullptr_t) = delete;

    // Move operators.
    unified& operator=(unified&& other) noexcept
    { return this->template operator=<unified, T, multiplicity_type>(std::move(other)); }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<internal::strong<Y, M>,
                                                        StdSmart<Y, M>>)>
    unified& operator=(StdSmart<Y, M>&& other) noexcept (parent::IsOptional)
    {
        if constexpr (std::is_base_of_v<unified<Y, M>, StdSmart<Y, M>>)
        {
            unified{std::move(other)}.swap_nothrow(*this);
        }
        else
        {
            unified{other}.swap_nothrow(*this);
            internal::utilize(std::move(other));
        }

        return *this;
    }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<weak<Y, M>,
                                                        StdSmart<Y, M>>)>
    unified& operator=(StdSmart<Y, M>&& other)
    {
        unified{other}.swap_nothrow(*this);
        internal::utilize(std::move(other));
        return *this;
    }

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unified& operator=(SharedReferrer<Y>&& referrer) noexcept (parent::IsOptional)
    {
        unified{std::move(referrer)}.swap_nothrow(*this);
        return *this;
    }

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unified& operator=(WeakReferrer<Y>&& referrer) noexcept (parent::IsOptional)
    {
        unified{std::move(referrer)}.swap_nothrow(*this);
        return *this;
    }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unified& operator=(StdSmart<Y, M>&& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unified& operator=(StdSmart<Y, M>&& other) = delete;

    void     swap(unified& other) noexcept (parent::IsOptional)
    { parent::swap(other); }
};

template <class T, class Multiplicity>
class unique : public internal::strict<T, Multiplicity>
{
private:
    using parent = internal::strict<T, Multiplicity>;
    using typename parent::multiplicity_type;

    template <class Y>
    static constexpr bool IsCompatible = internal::IsCompatible<T, Y>;

    template <class Y>
    static constexpr bool IsIncompatible = internal::IsIncompatible<T, Y>;

    template <class Y>
    static constexpr bool IsConstIncorrect = internal::IsConstIncorrect<T, Y>;

    template <class Y>
    using SharedReferrer = typename parent::template SharedReferrer<Y>;
    template <class Y>
    using WeakReferrer = typename parent::template WeakReferrer<Y>;

public:
    using parent::strict;
    using parent::operator=;

    // Copy constructors.
    unique(const unique& other) = delete;

    template <class Y, class M>
    unique(const unique<Y, M>& other) = delete;

    template <class Y, class M>
    unique(const unified<Y, M>& other) = delete;

    template <class Y, class M>
    unique(const shared<Y, M>& other) = delete;

    template <class Y, class M>
    unique(const weak<Y, M>& other) = delete;

    template <class Y>
    unique(const SharedReferrer<Y>& referrer) = delete;

    template <class Y>
    unique(const WeakReferrer<Y>& referrer) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unique(const StdSmart<Y, M>& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unique(const StdSmart<Y, M>& other) = delete;

    // Move constructors.
    unique(unique&& other) = default;

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unique(unique<Y, M>&& other) noexcept (parent::IsOptional)
        : parent{std::move(other)} {}

    template <class Y, class M>
    unique(unified<Y, M>&& other) = delete;

    template <class Y, class M>
    unique(shared<Y, M>&& other) = delete;

    template <class Y, class M>
    unique(weak<Y, M>&& other) = delete;

    template <class Y>
    unique(SharedReferrer<Y>&& referrer) = delete;

    template <class Y>
    unique(WeakReferrer<Y>&& referrer) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unique(StdSmart<Y, M>&& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unique(StdSmart<Y, M>&& other) = delete;

    // Copy operators.
    unique& operator=(const unique& other) = delete;

    template <class Y, class M>
    unique& operator=(const unique<Y, M>& other) = delete;

    template <class Y, class M>
    unique& operator=(const internal::base<Y, M>& other) = delete;

    template <class Y, class M>
    unique& operator=(const unified<Y, M>& other) = delete;

    template <class Y>
    unique& operator=(const SharedReferrer<Y>& referrer) = delete;

    template <class Y>
    unique& operator=(const WeakReferrer<Y>& referrer) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unique& operator=(const StdSmart<Y, M>& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unique& operator=(const StdSmart<Y, M>& other) = delete;

    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    unique& operator=(std::nullptr_t) noexcept
    {
        internal::utilize(std::move(*this));
        return *this;
    }

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    unique& operator=(std::nullptr_t) = delete;

    // Move operators.
    unique& operator=(unique&& other) noexcept
    { return this->template operator=<T, multiplicity_type>(std::move(other)); }

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unique& operator=(unique<Y, M>&& other)
    {
        unique{std::move(other)}.swap_nothrow(*this);
        return *this;
    }

    template <class Y, class M>
    unique& operator=(unified<Y, M>&& other) = delete;

    template <class Y, class M>
    unique& operator=(shared<Y, M>&& other) = delete;

    template <class Y, class M>
    unique& operator=(weak<Y, M>&& other) = delete;

    template <class Y>
    unique& operator=(SharedReferrer<Y>&& referrer) = delete;

    template <class Y>
    unique& operator=(WeakReferrer<Y>&& referrer) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unique& operator=(StdSmart<Y, M>&& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unique& operator=(StdSmart<Y, M>&& other) = delete;

    void    swap(unique& other) noexcept (parent::IsOptional)
    { parent::swap(other); }
};

template <class T, class Multiplicity>
class shared : public internal::strict<T, Multiplicity>
{
private:
    using parent = internal::strict<T, Multiplicity>;
    using typename parent::multiplicity_type;

    template <class Y>
    static constexpr bool IsCompatible = internal::IsCompatible<T, Y>;

    template <class Y>
    static constexpr bool IsIncompatible = internal::IsIncompatible<T, Y>;

    template <class Y>
    static constexpr bool IsConstIncorrect = internal::IsConstIncorrect<T, Y>;

    template <class Y>
    using SharedReferrer = typename parent::template SharedReferrer<Y>;
    template <class Y>
    using WeakReferrer = typename parent::template WeakReferrer<Y>;
    using Referrer     = typename parent::Referrer;

public:
    using parent::strict;
    using parent::operator=;

    // Copy constructors.
    shared(const shared& other) noexcept (parent::IsOptional)
        : parent{other} {}

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared(const shared<Y, M>& other) noexcept (parent::IsOptional)
        : parent{other} {}

    template <class Y, class M>
    shared(const unique<Y, M>& other) = delete;

    template <class Y, class M>
    shared(const unified<Y, M>& other) = delete;

    template <class Y, class M>
    shared(const weak<Y, M>& other) = delete;

    template <class Y>
    shared(const SharedReferrer<Y>& referrer) noexcept (parent::IsOptional)
        : parent{referrer} {}

    template <class Y>
    explicit shared(const WeakReferrer<Y>& referrer)
        : parent{referrer} {}

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    shared(const StdSmart<Y, M>& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    shared(const StdSmart<Y, M>& other) = delete;

    // Move constructors.
    shared(shared&& other) = default;

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared(shared<Y, M>&& other) noexcept (parent::IsOptional)
        : parent{std::move(other)} {}

    template <class Y, class M>
    shared(unified<Y, M>&& other) = delete;

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared(unique<Y, M>&& other) noexcept (parent::IsOptional)
        : parent{std::move(other)} {}

    template <class Y, class M>
    shared(weak<Y, M>&& other) = delete;

    template <class Y>
    shared(SharedReferrer<Y>&& referrer) noexcept (parent::IsOptional)
        : parent{std::move(referrer)} {}

    template <class Y>
    explicit shared(WeakReferrer<Y>&& referrer)
        : shared{static_cast<const WeakReferrer<Y>&>(referrer)}
    { referrer.reset(); }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    shared(StdSmart<Y, M>&& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    shared(StdSmart<Y, M>&& other) = delete;

    // Copy operators.
    shared& operator=(const shared& other) noexcept (parent::IsOptional)
    {
        return this->template operator=<T, multiplicity_type>(
            static_cast<const shared<T, multiplicity_type>&>(other));
    }

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared& operator=(const shared<Y, M>& other) noexcept (parent::IsOptional)
    {
        shared{other}.swap_nothrow(*this);
        return *this;
    }

    template <class Y, class M>
    shared& operator=(const unique<Y, M>& other) = delete;

    template <class Y, class M>
    shared& operator=(const unified<Y, M>& other) = delete;

    template <class Y, class M>
    shared& operator=(const weak<Y, M>& other) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared& operator=(const SharedReferrer<Y>& referrer) noexcept (parent::IsOptional)
    {
        shared{referrer}.swap_nothrow(*this);
        return *this;
    }

    template <class Y>
    shared& operator=(const WeakReferrer<Y>& referrer) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    shared& operator=(const StdSmart<Y, M>& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    shared& operator=(const StdSmart<Y, M>& other) = delete;

    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    shared& operator=(std::nullptr_t) noexcept
    {
        internal::utilize(std::move(*this));
        return *this;
    }

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    shared& operator=(std::nullptr_t) = delete;

    // Move operators.
    shared& operator=(shared&& other) noexcept
    { return this->template operator=<shared, T, multiplicity_type>(std::move(other)); }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared& operator=(StdSmart<Y, M>&& other)
    {
        shared{std::move(other)}.swap_nothrow(*this);
        return *this;
    }

    template <class Y, class M>
    shared& operator=(unified<Y, M>&& other) = delete;

    template <class Y, class M>
    shared& operator=(weak<Y, M>&& other) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared& operator=(SharedReferrer<Y>&& referrer) noexcept (parent::IsOptional)
    {
        shared{std::move(referrer)}.swap_nothrow(*this);
        return *this;
    }

    template <class Y>
    shared& operator=(WeakReferrer<Y>&& referrer) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    shared& operator=(StdSmart<Y, M>&& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    shared& operator=(StdSmart<Y, M>&& other) = delete;

    operator Referrer() const & noexcept
    { return this->copy_referrer(); }

    operator Referrer() && noexcept
    { return this->move_referrer(); }

    template <class Y>
    operator SharedReferrer<Y>() const & noexcept
    { return std::static_pointer_cast<Y>(this->copy_referrer()); }

    template <class Y>
    operator SharedReferrer<Y>() && noexcept
    { return SharedReferrer<Y>{this->move_referrer()}; }

    template <class Y>
    operator WeakReferrer<Y>() const & noexcept
    { return WeakReferrer<Y>{this->copy_referrer()}; }

    template <class Y>
    operator WeakReferrer<Y>() && = delete;

    void swap(shared& other) noexcept (parent::IsOptional)
    { parent::swap(other); }
};

template <class T, class Multiplicity>
class weak : public internal::weak<T, Multiplicity>
{
private:
    using parent = internal::weak<T, Multiplicity>;
    using typename parent::multiplicity_type;

    template <class Y>
    static constexpr bool IsCompatible = internal::IsCompatible<T, Y>;

    template <class Y>
    static constexpr bool IsIncompatible = internal::IsIncompatible<T, Y>;

    template <class Y>
    static constexpr bool IsConstIncorrect = internal::IsConstIncorrect<T, Y>;

    template <class Y>
    using SharedReferrer = typename parent::template SharedReferrer<Y>;
    template <class Y>
    using WeakReferrer = typename parent::template WeakReferrer<Y>;

public:
    using parent::weak;
    using parent::operator=;

    // Copy constructors.
    weak(const weak& other) noexcept
        : parent{other} {}

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    weak(const weak<Y, M>& other) noexcept
        : parent{other} {}

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    weak(const internal::strong<Y, M>& other) noexcept
        : parent{other} {}

    template <class Y>
    weak(const SharedReferrer<Y>& referrer) noexcept (parent::IsOptional)
        : parent{referrer} {}

    template <class Y>
    weak(const WeakReferrer<Y>& referrer) noexcept
        : parent{referrer} {}

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    weak(const StdSmart<Y, M>& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    weak(const StdSmart<Y, M>& other) = delete;

    // Move constructors.
    weak(weak&& other) = default;

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    weak(weak<Y, M>&& other) noexcept (parent::IsOptional)
        : parent{std::move(other)} {}

    weak(internal::strong<T, multiplicity_type>&& other) = delete;

    template <class Y>
    weak(WeakReferrer<Y>&& referrer) noexcept
        : parent{std::move(referrer)} {}

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    weak(StdSmart<Y, M>&& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    weak(StdSmart<Y, M>&& other) = delete;

    // Copy operators.
    weak& operator=(const weak& other) noexcept
    { return this->template operator=<weak, T, multiplicity_type>(other); }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    weak& operator=(const StdSmart<Y, M>& other) noexcept
    {
        weak{other}.swap(*this);
        return *this;
    }

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    weak& operator=(const SharedReferrer<Y>& referrer) noexcept (parent::IsOptional)
    {
        weak{referrer}.swap(*this);
        return *this;
    }

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    weak& operator=(const WeakReferrer<Y>& referrer) noexcept (parent::IsOptional)
    {
        weak{referrer}.swap(*this);
        return *this;
    }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    weak& operator=(const StdSmart<Y, M>& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    weak& operator=(const StdSmart<Y, M>& other) = delete;

    // Move operators.
    weak& operator=(weak&& other) noexcept
    { return this->template operator=<weak, T, multiplicity_type>(std::move(other)); }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<weak<Y, M>,
                                                        StdSmart<Y, M>>)>
    weak& operator=(StdSmart<Y, M>&& other)
    {
        weak{std::move(other)}.swap(*this);
        return *this;
    }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<internal::strong<Y, M>,
                                                        StdSmart<Y, M>>)>
    weak& operator=(StdSmart<Y, M>&& other) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    weak& operator=(WeakReferrer<Y>&& referrer) noexcept (parent::IsOptional)
    {
        weak{std::move(referrer)}.swap(*this);
        return *this;
    }

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    weak& operator=(StdSmart<Y, M>&& other) = delete;

    template <template <class Y, class M> class StdSmart, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    weak& operator=(StdSmart<Y, M>&& other) = delete;

    unified<T, tag::optional> lock() const noexcept
    { return unified<T, tag::optional>{parent::lock()}; }

    void swap(weak& other) noexcept (parent::IsOptional)
    { parent::swap(other); }
};

} // namespace std_smart

} // namespace detail

} // namespace v0_1

} // namespace upl
