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

#include <upl/v0_2/detail/internal/utility/concept.h>

#include <stdexcept>

#define UPL_TRACK_UNIQUE_CARRIER_x

#ifdef UPL_TRACK_UNIQUE_CARRIER
#include <iostream>
#endif

namespace upl
{

inline namespace v0_2
{

template <class T>
class unique_carrier
{
public:
    unique_carrier() = delete;

    unique_carrier(const T& holder) = delete;
    unique_carrier(T&& holder) noexcept : m_holder{std::move(holder)} {}

    unique_carrier(const unique_carrier& other)
        : m_is_moved{other.m_is_moved},
          m_holder{std::move(const_cast<unique_carrier&>(other).release())}
    {
    #ifdef UPL_TRACK_UNIQUE_CARRIER
        std::cout << "coping a unique_carrier" << std::endl;
    #endif
    }
    // { throw std::logic_error{"coping a unique_carrier"}; }

    unique_carrier(unique_carrier&&) = default;

    operator T && () && { return release(); }
    // TODO: Examine the 'operator T &&() &' for the std::bind.
    // operator T && () & = delete;
    operator T && () & { return release(); }

    template <class U, UPL_CONCEPT_REQUIRES_(std::is_convertible_v<T, U>)>
    operator U() && { return release(); }

    template <class U, UPL_CONCEPT_REQUIRES_(std::is_convertible_v<T, U>)>
    operator U()& = delete;

    auto& operator*()
    {
        if (m_is_moved)
            throw std::logic_error{"accessing an empty unique_carrier"};

        return *m_holder;
    }

protected:
    T && release()
    {
        if (!m_is_moved)
        {
            m_is_moved = true;
            return std::move(m_holder);
        }

        throw std::logic_error{"moving an empty unique_carrier"};
    }

private:
    bool m_is_moved{false};
    T    m_holder;
};

template <class T>
unique_carrier(T &&)->unique_carrier<T>;

} // namespace v0_2

} // namespace upl
