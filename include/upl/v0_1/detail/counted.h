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
#include <upl/v0_1/utility/concept.h>

#include <cassert>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>

namespace upl
{
inline namespace v0_1
{
namespace detail
{
namespace counted
{
namespace internal
{
struct owner_counters
{
    static const std::size_t unique_magic =
        std::numeric_limits<std::size_t>::max();

    std::size_t strong_count{0};
    std::size_t weak_count{0};
    std::size_t unified_count{0};
};

struct initial_counters
{
    static const std::size_t unique_magic =
        owner_counters::unique_magic;

    static constexpr owner_counters unique{unique_magic, 0, 0};
    static constexpr owner_counters shared{1, 0, 0};
    static constexpr owner_counters unified{0, 0, 1};
};

class owner_control_block
{
public:
    owner_control_block(const owner_counters& counters)
        : m_counters{counters}
    {}

    bool has_strong() const noexcept
    {
        return m_counters.strong_count != 0;
    }

    bool has_weak() const noexcept
    {
        return m_counters.weak_count != 0;
    }

    bool has_owner() const noexcept
    {
        return m_counters.strong_count != 0 || m_counters.unified_count != 0;
    }

    bool is_unique() const noexcept
    {
        return m_counters.strong_count == owner_counters::unique_magic;
    }

    void set_unique()
    {
        assert(m_counters.strong_count <= 1);

        m_counters.strong_count = owner_counters::unique_magic;
    }

    void unset_unique()
    {
        assert(m_counters.strong_count == owner_counters::unique_magic);

        m_counters.strong_count = 0;
    }

    void convert_to_unique()
    {
        assert(m_counters.strong_count != owner_counters::unique_magic);

        if (m_counters.strong_count <= 1)
            m_counters.strong_count = owner_counters::unique_magic;
        else
            throw shared_error{"non-unique 'shared' "
                               "can't be converted to an 'unique'"};
    }

    void increment_shared()
    {
        assert(  m_counters.strong_count != owner_counters::unique_magic
              && m_counters.strong_count + 1 != owner_counters::unique_magic);

        ++m_counters.strong_count;
    }

    void decrement_shared()
    {
        assert(  m_counters.strong_count != owner_counters::unique_magic
              && m_counters.strong_count > 0);

        --m_counters.strong_count;
    }

    void convert_to_shared()
    {
        assert(is_unique());

        m_counters.strong_count = 1;
    }

    void increment_weak()
    {
        ++m_counters.weak_count;
    }

    void decrement_weak()
    {
        assert(m_counters.weak_count > 0);

        --m_counters.weak_count;
    }

    bool increment_unified()
    {
        if (!has_owner())
            return false;

        ++m_counters.unified_count;
        return true;
    }

    void decrement_unified()
    {
        assert(m_counters.unified_count > 0);

        --m_counters.unified_count;
    }

private:
    // TODO: Make the m_counters thread safe.
    owner_counters m_counters;
};

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
} // namespace

template <template <class Y, class M> class Counted, class Y, class M>
inline void utilize(Counted<Y, M>&& other) noexcept
{ Counted<Y, tag::optional>{std::move(other)}; }

template <class T, class Multiplicity>
class base
{
protected:
    using multiplicity_type = Multiplicity;

    static constexpr bool IsOptional =
        std::is_same_v<multiplicity_type, tag::optional>;
    static constexpr bool IsSingle =
        std::is_same_v<multiplicity_type, tag::single>;

public:
    using element_type = T;

    template <class U, class M>
    bool owner_before(const base<U, M>& other) const noexcept (IsOptional && base<U, M>::IsOptional)
    {
        // TODO: Should this throw an exception?
        if constexpr (IsSingle)
            this->check_empty_single_access();

        return other.owner_before_inverse(*this);
    }

protected:
    // Default constructors.
    UPL_CONCEPT_REQUIRES(IsOptional)
    constexpr base() noexcept
        : m_data{},
          m_control_block{}
    {}

    UPL_CONCEPT_REQUIRES(IsSingle)
    base() = delete;

    // Value constructors.
    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    base(Y* data, const owner_counters& counters)
    {
        if constexpr (IsSingle)
            if (data == nullptr)
                throw single_error{"'single' can't be created "
                                   "from a null pointer."};

        m_data = data;
        m_control_block = new owner_control_block{counters};
    }

    template <class Y, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<T, Y>)>
    base(Y* data, const owner_counters& counters) = delete;

    // Copy constructors.
    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    base(const base<Y, M>& other) noexcept (IsOptional)
        : m_data{other.m_data},
          m_control_block{other.m_control_block}
    {
        if constexpr (IsSingle)
            if (!this->has_data())
                throw single_error{"'single' can't be copied "
                                   "from a null pointer"};
    }

    // Move constructors.
    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    base(base<Y, M>&& other) noexcept (IsOptional)
        : base{static_cast<const base<Y, M>&>(other)}
    {
        other.m_data = nullptr;
        other.m_control_block = nullptr;
    }

    ~base() = default;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    void swap(base<Y, multiplicity_type>& other) noexcept (IsOptional)
    {
        if constexpr (IsSingle)
            check_empty_single_swap();

        other.swap_inverse(*this);
    }

    template <class Y>
    void swap_nothrow(base<Y, multiplicity_type>& other) noexcept
    {
        T* tmp_data = other.m_data;
        other.m_data = static_cast<Y*>(m_data);
        m_data       = tmp_data;
        std::swap(m_control_block, other.m_control_block);
    }

    element_type* data() const noexcept     { return m_data; }

    bool has_data() const noexcept          { return m_data != nullptr; }
    bool has_control_block() const noexcept { return m_control_block != nullptr; }

    bool has_strong() const noexcept
    { return has_control_block() && control_block().has_strong(); }

    bool has_owner() const noexcept
    { return has_control_block() && control_block().has_owner(); }

    bool is_unique() const noexcept
    { return has_control_block() && control_block().is_unique(); }

    void detach() noexcept
    {
        m_data = nullptr;
        m_control_block = nullptr;
    }

    void set_unique_ownership()
    {
        if (has_control_block())
            control_block().set_unique();
    }

    void unset_unique_ownership()
    {
        if (has_control_block())
        {
            control_block().unset_unique();

            adjust_strong_consistency();
        }
    }

    void convert_to_unique_ownership()
    {
        if (has_control_block())
            control_block().convert_to_unique();
    }

    void increment_shared_ownership()
    {
        if (has_control_block())
            control_block().increment_shared();
    }

    void decrement_shared_ownership()
    {
        if (has_control_block())
        {
            control_block().decrement_shared();

            adjust_strong_consistency();
        }
    }

    void convert_to_shared_ownership()
    {
        if (has_control_block())
            control_block().convert_to_shared();
    }

    void increment_weak_ownership()
    {
        if (has_control_block())
            control_block().increment_weak();
    }

    void decrement_weak_ownership()
    {
        if (has_control_block())
        {
            control_block().decrement_weak();

            adjust_weak_consistency();
        }
    }

    bool increment_unified_ownership()
    {
        return has_control_block() && control_block().increment_unified();
    }

    void decrement_unified_ownership()
    {
        if (has_control_block())
        {
            control_block().decrement_unified();

            adjust_strong_consistency();
        }
    }

    UPL_CONCEPT_REQUIRES(IsSingle)
    void check_empty_single_access() const
    {
        if (!this->has_data())
            this->handle_empty_single_access();
    }

    UPL_CONCEPT_REQUIRES(IsSingle)
    void check_empty_single_swap() const
    {
        if (!this->has_data())
            this->handle_empty_single_swap();
    }

    UPL_CONCEPT_REQUIRES(IsSingle)
    void handle_empty_single_access() const
    { throw single_error{"'single' is empty"}; }

    UPL_CONCEPT_REQUIRES(IsSingle)
    void handle_empty_single_swap() const
    { throw single_error{"'single' can't be swapped with a null pointer"}; }

private:
    template <class U, class M>
    bool owner_before_inverse(const base<U, M>& other) const noexcept (IsOptional)
    {
        // TODO: Should this throw an exception?
        if constexpr (IsSingle)
            this->check_empty_single_access();

        return std::less<owner_control_block*>()(other.m_control_block,
                                                 this->m_control_block);
    }

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<T, Y>)>
    void swap_inverse(base<Y, multiplicity_type>& other) noexcept (IsOptional)
    {
        if constexpr (IsSingle)
            check_empty_single_swap();

        this->swap_nothrow(other);
    }

    void adjust_strong_consistency()
    {
        assert(has_control_block());

        if (!control_block().has_owner())
        {
            delete_data();

            if (!control_block().has_weak())
                delete_control_block();
        }
    }

    void adjust_weak_consistency()
    {
        assert(has_control_block());

        if (!control_block().has_owner())
        {
            m_data = nullptr;

            if (!control_block().has_weak())
                delete_control_block();
        }
    }

    void delete_data()
    {
        if (has_data())
        {
            delete m_data;
            m_data = nullptr;
        }
    }

    void delete_control_block()
    {
        if (has_control_block())
        {
            delete m_control_block;
            m_control_block = nullptr;
        }
    }

    owner_control_block& control_block() noexcept
    {
        assert(has_control_block());
        return *m_control_block;
    }

    const owner_control_block& control_block() const noexcept
    {
        assert(has_control_block());
        return *m_control_block;
    }

    template <class Y, class multiplicity_type>
    friend class base;

    element_type* m_data{nullptr};
    owner_control_block* m_control_block{nullptr};
};

template <class T, class Multiplicity>
class strong : public base<T, Multiplicity>
{
protected:
    using parent = base<T, Multiplicity>;
    using typename parent::multiplicity_type;

public:
    using typename base<T, multiplicity_type>::element_type;

    // Default constructor.
    strong() = default;

    // Value constructors.
    constexpr strong(std::nullptr_t) noexcept
        : strong{}
    {}

    // Copy constructors.
    template <class Y, class M>
    strong(const base<Y, M>& other) noexcept (parent::IsOptional)
        : base<T, multiplicity_type>{other}
    {}

    // Move constructors.
    template <class Y, class M>
    strong(base<Y, M>&& other) noexcept (parent::IsOptional)
        : base<T, multiplicity_type>{std::move(other)}
    {}

    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    explicit operator bool() const noexcept { return this->has_data(); }
    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    explicit constexpr operator bool() const noexcept { return true; }

    /*! Returns the stored pointer. */
    element_type* get() const noexcept (parent::IsOptional)
    {
        // TODO: Should this throw an exception?
        if constexpr (parent::IsSingle)
            this->check_empty_single_access();

        return this->data();
    }

    /*! Dereferences the stored pointer. */
    T& operator*() const noexcept (parent::IsOptional)  { return *get(); }
    /*! Dereferences the stored pointer. */
    T* operator->() const noexcept (parent::IsOptional) { return get(); }

protected:
    using parent::base;
    using parent::operator=;
};

template <class T, class Multiplicity>
class weak : public base<T, Multiplicity>
{
protected:
    using parent = base<T, Multiplicity>;
    using typename parent::multiplicity_type;

public:
    // Default constructor.
    weak() = default;

    // Copy constructors.
    template <class Y, class M>
    weak(const base<Y, M>& other) noexcept (parent::IsOptional)
        : base<T, multiplicity_type>{other}
    {}

    // Move constructors.
    template <class Y, class M>
    weak(strong<Y, M>&& other) noexcept (parent::IsOptional)
        : base<T, multiplicity_type>{std::move(other)}
    {}

protected:
    using parent::base;
    using parent::operator=;
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

public:
    using parent::strong;
    using parent::operator=;

    // Value constructors.
    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    explicit unified(Y* data)
        : parent{data, internal::initial_counters::unified}
    {}
    template <class Y, UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unified(Y* data) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unified(Y*) = delete;

    // In-place constructors.
    template <class ... Values, UPL_CONCEPT_REQUIRES_(!std::is_abstract_v<T>)>
    explicit unified(std::in_place_t, Values&& ... values)
        : unified<T, multiplicity_type>{new T(std::forward<Values>(values) ...)}
    {}

    template <class ... Values, UPL_CONCEPT_REQUIRES_(std::is_abstract_v<T>)>
    unified(std::in_place_t, Values&& ... values) = delete;

    // Copy constructors.
    unified(const unified& other) noexcept (parent::IsOptional)
        : unified{static_cast<const internal::strong<T, multiplicity_type>&>(other)}
    {}

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unified(const internal::strong<Y, M>& other) noexcept (parent::IsOptional)
        : parent{other}
    { this->increment_unified_ownership(); }

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unified(const weak<Y, M>& other)
        : parent{other}
    {
        // TODO: Optimize.
        if (this->has_control_block() && !this->has_owner())
            throw weak_error{"'unified' can't be copied from expired 'weak'"};

        this->increment_unified_ownership();
    }

    // TODO: Hide the constructor special for the weak::lock().
    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unified(const weak<Y, M>& other, std::nothrow_t) noexcept
        : parent{other}
    {
        // TODO: Optimize.
        if (!this->increment_unified_ownership())
            this->detach();
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unified(const Counted<Y, M>& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unified(const Counted<Y, M>& other) = delete;

    // Move constructors.
    unified(unified&& other) = default;

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unified(unified<Y, M>&& other) noexcept (parent::IsOptional)
        : parent{std::move(other)}
    {}

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && (  std::is_base_of_v<unique<Y, M>, Counted<Y, M>>
                                      || std::is_base_of_v<shared<Y, M>, Counted<Y, M>>))>
    unified(Counted<Y, M>&& other) noexcept (parent::IsOptional)
        : unified{static_cast<const internal::strong<Y, M>&>(other)}
    { internal::utilize(std::move(other)); }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<weak<Y, M>, Counted<Y, M>>)>
    unified(Counted<Y, M>&& other)
        : unified{static_cast<const weak<Y, M>&>(other)}
    { internal::utilize(std::move(other)); }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unified(Counted<Y, M>&& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unified(Counted<Y, M>&& other) = delete;

    // Copy operators.
    unified& operator=(const unified& other) noexcept
    {
        return this->operator=<internal::strong, T>(
            static_cast<const internal::strong<T, multiplicity_type>&>(other));
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<internal::strong<Y, M>,
                                                        Counted<Y, M>>)>
    unified& operator=(const Counted<Y, M>& other) noexcept (parent::IsOptional)
    {
        unified{other}.swap_nothrow(*this);
        return *this;
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<weak<Y, M>, Counted<Y, M>>)>
    unified& operator=(const Counted<Y, M>& other)
    {
        unified{other}.swap_nothrow(*this);
        return *this;
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unified& operator=(const Counted<Y, M>& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unified& operator=(const Counted<Y, M>& other) = delete;

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
    { return this->operator=<unified, T>(std::move(other)); }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<internal::strong<Y, M>,
                                                        Counted<Y, M>>)>
    unified& operator=(Counted<Y, M>&& other) noexcept (parent::IsOptional)
    {
        if constexpr (std::is_base_of_v<unified<Y, M>, Counted<Y, M>>)
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

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<weak<Y, M>,
                                                        Counted<Y, M>>)>
    unified& operator=(Counted<Y, M>&& other)
    {
        unified{other}.swap_nothrow(*this);
        internal::utilize(std::move(other));
        return *this;
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unified& operator=(Counted<Y, M>&& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unified& operator=(Counted<Y, M>&& other) = delete;

    ~unified() { this->decrement_unified_ownership(); }

    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    void reset() noexcept { internal::utilize(std::move(*this)); }

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    void reset() = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    void reset(Y* data)
    {
        assert(this->get() == nullptr || this->get() != data);
        unified{data}.swap_nothrow(*this);
    }

    void swap(unified& other) noexcept (parent::IsOptional)
    { parent::swap(other); }
};

template <class T, class Multiplicity>
class unique : public internal::strong<T, Multiplicity>
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

public:
    using parent::strong;
    using parent::operator=;

    // Value constructors.
    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    explicit unique(Y* data)
        : parent{data, internal::initial_counters::unique}
    {}

    template <class Y, UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unique(Y* data) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unique(Y*) = delete;

    // In-place constructors.
    template <class ... Values, UPL_CONCEPT_REQUIRES_(!std::is_abstract_v<T>)>
    explicit unique(std::in_place_t, Values&& ... values)
        : unique<T, multiplicity_type>{new T(std::forward<Values>(values) ...)}
    {}

    template <class ... Values, UPL_CONCEPT_REQUIRES_(std::is_abstract_v<T>)>
    unique(std::in_place_t, Values&& ... values) = delete;

    // Copy constructors.
    unique(const unique& other) = delete;

    template <class Y, class M>
    unique(const unique<Y, M>& other) = delete;

    template <class Y, class M>
    unique(const internal::base<Y, M>& other) = delete;

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unique(const unified<Y, M>& other)
        : parent{other}
    {
        if (this->has_strong())
            throw strong_error{"'unique' can't be copied from a strong owner"};

        this->set_unique_ownership();
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unique(const Counted<Y, M>& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unique(const Counted<Y, M>& other) = delete;

    // Move constructors.
    unique(unique&& other) = default;

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unique(unique<Y, M>&& other) noexcept (parent::IsOptional)
        : parent{std::move(other)}
    {}

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unique(internal::strong<Y, M>&& other)
        : parent{other}
    {
        this->convert_to_unique_ownership();
        internal::utilize(std::move(other));
    }

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unique(unified<Y, M>&& other)
        : unique{static_cast<const unified<Y, M>&>(other)}
    { internal::utilize(std::move(other)); }

    template <class Y, class M>
    unique(weak<Y, M>&& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unique(Counted<Y, M>&& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unique(Counted<Y, M>&& other) = delete;

    // Copy operators.
    unique& operator=(const unique& other) = delete;

    template <class Y, class M>
    unique& operator=(const unique<Y, M>& other) = delete;

    template <class Y, class M>
    unique& operator=(const internal::base<Y, M>& other) = delete;

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    unique& operator=(const unified<Y, M>& other)
    {
        unique{other}.swap_nothrow(*this);
        return *this;
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unique& operator=(const Counted<Y, M>& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unique& operator=(const Counted<Y, M>& other) = delete;

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
    { return this->operator=<unique, T>(std::move(other)); }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && !std::is_base_of_v<weak<Y, M>,
                                                         Counted<Y, M>>)>
    unique& operator=(Counted<Y, M>&& other)
    {
        unique{std::move(other)}.swap_nothrow(*this);
        return *this;
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<weak<Y, M>,
                                                        Counted<Y, M>>)>
    unique& operator=(Counted<Y, M>&& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    unique& operator=(Counted<Y, M>&& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    unique& operator=(Counted<Y, M>&& other) = delete;

    ~unique() { this->unset_unique_ownership(); }

    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    void reset() noexcept { internal::utilize(std::move(*this)); }

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    void reset() = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    void reset(Y* data)
    {
        assert(this->get() == nullptr || this->get() != data);
        unique{data}.swap_nothrow(*this);
    }

    void swap(unique& other) noexcept (parent::IsOptional)
    { parent::swap(other); }
};

template <class T, class Multiplicity>
class shared : public internal::strong<T, Multiplicity>
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

public:
    using parent::strong;
    using parent::operator=;

    // Value constructors.
    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    explicit shared(Y* data)
        : parent{data, internal::initial_counters::shared}
    {}

    template <class Y, UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    shared(Y* data) = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    shared(Y*) = delete;

    // In-place constructors.
    template <class ... Values, UPL_CONCEPT_REQUIRES_(!std::is_abstract_v<T>)>
    explicit shared(std::in_place_t, Values&& ... values)
        : shared<T, multiplicity_type>{new T(std::forward<Values>(values) ...)}
    {}

    template <class ... Values, UPL_CONCEPT_REQUIRES_(std::is_abstract_v<T>)>
    shared(std::in_place_t, Values&& ... values) = delete;

    // Copy constructors.
    shared(const shared& other) noexcept (parent::IsOptional)
        : parent{other}
    { this->increment_shared_ownership(); }

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared(const shared<Y, M>& other) noexcept (parent::IsOptional)
        : parent{other}
    { this->increment_shared_ownership(); }

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared(const internal::base<Y, M>& other)
        : parent{other}
    {
        // TODO: Optimize.
        if (this->is_unique())
            throw unique_error{"'shared' can't be copied from an 'unique'"};

        if (this->has_control_block() && !this->has_owner())
            throw weak_error{"'shared' can't be copied from expired 'weak'"};

        this->increment_shared_ownership();
    }

    template <class Y, class M>
    shared(const unique<Y, M>& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    shared(const Counted<Y, M>& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    shared(const Counted<Y, M>& other) = delete;

    // Move constructors.
    shared(shared&& other) = default;

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared(shared<Y, M>&& other) noexcept (parent::IsOptional)
        : parent{std::move(other)}
    {}

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared(unified<Y, M>&& other)
        : shared{static_cast<const unified<Y, M>&>(other)}
    { unified<Y, M>{std::move(other)}; }

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared(unique<Y, M>&& other) noexcept (parent::IsOptional)
        : parent{other}
    {
        this->convert_to_shared_ownership();
        internal::strong<Y, M>{std::move(other)};
    }

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared(weak<Y, M>&& other)
        : shared{static_cast<const weak<Y, M>&>(other)}
    { weak<Y, M>{std::move(other)}; }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    shared(Counted<Y, M>&& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    shared(Counted<Y, M>&& other) = delete;

    // Copy operators.
    shared& operator=(const shared& other) noexcept
    {
        return this->operator=<internal::base, T>(
            static_cast<const internal::base<T, multiplicity_type>&>(other));
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<internal::base<Y, M>,
                                                        Counted<Y, M>>)>
    shared& operator=(const Counted<Y, M>& other)
    {
        shared{other}.swap_nothrow(*this);
        return *this;
    }

    template <class Y, class M>
    shared& operator=(const unique<Y, M>& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    shared& operator=(const Counted<Y, M>& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    shared& operator=(const Counted<Y, M>& other) = delete;

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
    { return this->operator=<shared, T>(std::move(other)); }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    shared& operator=(Counted<Y, M>&& other)
    {
        shared{std::move(other)}.swap_nothrow(*this);
        return *this;
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    shared& operator=(Counted<Y, M>&& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    shared& operator=(Counted<Y, M>&& other) = delete;

    ~shared() { this->decrement_shared_ownership(); }

    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    void reset() noexcept { internal::utilize(std::move(*this)); }

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    void reset() = delete;

    template <class Y, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    void reset(Y* data)
    {
        assert(this->get() == nullptr || this->get() != data);
        shared{data}.swap_nothrow(*this);
    }

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

public:
    using parent::weak;
    using parent::operator=;

    // Copy constructors.
    weak(const weak& other) noexcept (parent::IsOptional)
        : internal::weak<T, multiplicity_type>{other}
    { this->increment_weak_ownership(); }

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    weak(const weak<Y, M>& other) noexcept (parent::IsOptional)
        : internal::weak<T, multiplicity_type>{other}
    { this->increment_weak_ownership(); }

    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    weak(const internal::base<Y, M>& other) noexcept (parent::IsOptional)
        : internal::weak<T, multiplicity_type>{other}
    { this->increment_weak_ownership(); }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    weak(const Counted<Y, M>& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    weak(const Counted<Y, M>& other) = delete;

    // Move constructors.
    template <class Y, class M, UPL_CONCEPT_REQUIRES_(IsCompatible<Y>)>
    weak(weak<Y, M>&& other) noexcept (parent::IsOptional)
        : internal::weak<T, multiplicity_type>{std::move(other)}
    {}

    weak(internal::strong<T, multiplicity_type>&& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    weak(Counted<Y, M>&& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    weak(Counted<Y, M>&& other) = delete;

    // Copy operators.
    weak& operator=(const weak& other) noexcept
    {
        return this->operator=<internal::base, T>(
            static_cast<const internal::base<T, multiplicity_type>&>(other));
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<internal::base<Y, M>,
                                                        Counted<Y, M>>)>
    weak& operator=(const Counted<Y, M>& other) noexcept (parent::IsOptional)
    {
        weak{other}.swap_nothrow(*this);
        return *this;
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    weak& operator=(const Counted<Y, M>& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    weak& operator=(const Counted<Y, M>& other) = delete;

    // Move operators.
    weak& operator=(weak&& other) noexcept
    { return this->operator=<weak, T>(std::move(other)); }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<weak<Y, M>,
                                                        Counted<Y, M>>)>
    weak& operator=(Counted<Y, M>&& other)
    {
        weak{std::move(other)}.swap_nothrow(*this);
        return *this;
    }

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(  IsCompatible<Y>
                                   && std::is_base_of_v<internal::strong<Y, M>,
                                                        Counted<Y, M>>)>
    weak& operator=(Counted<Y, M>&& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsIncompatible<Y>)>
    weak& operator=(Counted<Y, M>&& other) = delete;

    template <template <class Y, class M> class Counted, class Y, class M,
              UPL_CONCEPT_REQUIRES_(IsConstIncorrect<Y>)>
    weak& operator=(Counted<Y, M>&& other) = delete;

    ~weak() { this->decrement_weak_ownership(); }

    bool expired() const noexcept (parent::IsOptional)
    {
        // TODO: Should this throw an exception?
        if constexpr (parent::IsSingle)
            this->check_empty_single_access();

        return !this->has_owner();
    }

    unified<T, tag::optional> lock() const noexcept (parent::IsOptional)
    {
        // TODO: Should this throw an exception?
        if constexpr (parent::IsSingle)
            this->check_empty_single_access();

        return unified<T, tag::optional>{*this, std::nothrow};
    }

    UPL_CONCEPT_REQUIRES(parent::IsOptional)
    void reset() noexcept { internal::utilize(std::move(*this)); }

    UPL_CONCEPT_REQUIRES(parent::IsSingle)
    void reset() = delete;

    void swap(weak& other) noexcept (parent::IsOptional)
    { parent::swap(other); }
};
} // namespace counted
} // namespace detail
} // namespace v0_1
} // namespace upl
