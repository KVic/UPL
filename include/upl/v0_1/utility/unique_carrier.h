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

#include <upl/v0_1/detail/std_smart/source/Utility/Concept.h>

#include <stdexcept>

namespace upl
{

inline namespace v0_1
{

template <class T>
class unique_carrier
{
public:
    unique_carrier() = delete;

    unique_carrier(const T& holder) = delete;
    unique_carrier(T&& holder) noexcept : m_holder{std::move(holder)} {}

    unique_carrier(const unique_carrier&) : m_holder{}
    { throw std::logic_error{"coping a unique_carrier"}; }

    unique_carrier(unique_carrier&&) = default;

    operator T && () && { return release(); }
    // TODO: Examine the 'operator T &&() &' for the std::bind.
    // operator T && () & = delete;
    operator T && () & { return release(); }

    template <class U, UPL_CONCEPT_REQUIRES_(std::is_convertible_v<T, U>)>
    operator U() && { return release(); }

    template <class U, UPL_CONCEPT_REQUIRES_(std::is_convertible_v<T, U>)>
    operator U()& = delete;

    auto& operator*() { return *m_holder; }

protected:
    T && release()
    {
        if (m_holder)
            return std::move(m_holder);
        else
            throw std::logic_error{"moving an empty unique_carrier"};
    }

private:
    T m_holder;
};

template <class T>
unique_carrier(T &&)->unique_carrier<T>;

} // namespace v0_1

} // namespace upl
