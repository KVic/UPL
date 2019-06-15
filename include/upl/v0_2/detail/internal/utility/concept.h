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

// *INDENT-OFF*

#define UPL_WORD_CONCAT_EXT(Word_1, Word_2) Word_1 ## Word_2
#define UPL_WORD_CONCAT(Word_1, Word_2) UPL_WORD_CONCAT_EXT(Word_1, Word_2)

#define UPL_NAME_BY_LINE(Name) UPL_WORD_CONCAT(Name, __LINE__)

#if defined (__cpp_concepts)
#define UPL_CONCEPT_SPECIFIER concept bool
#else
#define UPL_CONCEPT_SPECIFIER inline constexpr bool
#endif

#define UPL_CONCEPT_REQUIRES(...)                                              \
template <bool UPL_NAME_BY_LINE(_UPL_Requires_) = true,                        \
          typename std::enable_if_t<                                           \
              (__VA_ARGS__) && UPL_NAME_BY_LINE(_UPL_Requires_), int> = 0>     \

#define UPL_CONCEPT_REQUIRES_(...)                                             \
bool UPL_NAME_BY_LINE(_UPL_Requires_) = true,                                  \
typename std::enable_if_t<                                                     \
    (__VA_ARGS__) && UPL_NAME_BY_LINE(_UPL_Requires_), int> = 0                \

// *INDENT-ON*
