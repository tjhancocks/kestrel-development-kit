/*
* Copyright (c) 2019 Tom Hancocks
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

#include "rsrc/data.hpp"

// MARK: - Constructor

rsrc::data::data(rsrc::data::endian e)
    : m_endian(e)
{
    
}

// MARK: - Mutators

void rsrc::data::set_endian(rsrc::data::endian e)
{
    m_endian = e;
}

// MARK: - Endian

template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type*>
T rsrc::data::swap(T value, rsrc::data::endian mode) const
{
    T v = 0;
    auto size = sizeof(T);
    
    if (mode == big) {
        for (auto i = 0; i < size; ++i) {
            auto b = (size - 1 - i) << 3;
            v |= ((value >> b) & 0xFF) << (i << 3);
        }
    }
    
    return v;
}

// MARK: - Writers

template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type*>
void rsrc::data::write_value(const T& value)
{
    auto size = sizeof(T);
    auto swapped = swap(value, m_endian);
    
    for (auto i = 0; i < size; ++i) {
        auto b = (size - 1 - i) << 3;
        m_data.push_back((value >> b) & 0xFF);
    }
}
