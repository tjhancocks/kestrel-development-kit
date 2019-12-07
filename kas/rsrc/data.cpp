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

#include <iostream>
#include <fstream>
#include <algorithm>
#include "rsrc/data.hpp"
#include "rsrc/macroman.hpp"

// MARK: - Constructor

rsrc::data::data(rsrc::data::endian e)
    : m_endian(e)
{
    
}

rsrc::data::data(std::shared_ptr<std::vector<uint8_t>> blob)
    : m_endian(rsrc::data::endian::big), m_data(*blob)
{
    
}

// MARK: - File Access

void rsrc::data::save(const std::string& path) const
{
    std::cout << "Saving data to " << path << std::endl;
    std::ofstream f(path, std::ios::out | std::ios::binary);
    f.write((char *)&m_data[0], m_data.size());
    f.close();
}

// MARK: - Mutators

void rsrc::data::set_endian(rsrc::data::endian e)
{
    m_endian = e;
}

void rsrc::data::set_insertion_point(uint64_t p)
{
    m_ptr = p;
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
void rsrc::data::write_integer(const T& value)
{
    auto size = sizeof(T);
    auto swapped = swap(value, m_endian);
    
    for (auto i = 0; i < size; ++i) {
        auto b = (size - 1 - i) << 3;
        
        // Two modes for writing. If we're at the very end of data then we need to insert.
        // If we're not at the end of the data then we can either overwrite the current byte
        // or insert a new byte.
        if (m_ptr >= this->size()) {
            m_data.push_back((swapped >> b) & 0xFF);
            m_ptr++;
        }
        else {
            m_data[m_ptr++] = (swapped >> b) & 0xFF;
        }
    }
}

void rsrc::data::write_byte(uint8_t v)
{
    write_integer(v);
}

void rsrc::data::write_signed_byte(int8_t v)
{
    write_integer(v);
}

void rsrc::data::write_byte(uint8_t v, uint64_t n)
{
    for (auto i = 0; i < n; ++i) {
        write_byte(v);
    }
}

void rsrc::data::write_word(uint16_t v)
{
    write_integer(v);
}

void rsrc::data::write_signed_word(int16_t v)
{
    write_integer(v);
}

void rsrc::data::write_long(uint32_t v)
{
    write_integer(v);
}

void rsrc::data::write_signed_long(int32_t v)
{
    write_integer(v);
}

void rsrc::data::write_quad(uint64_t v)
{
    write_integer(v);
}

void rsrc::data::write_signed_quad(int64_t v)
{
    write_integer(v);
}

void rsrc::data::write_pstr(const std::string& str)
{
   
    auto mac_roman = rsrc::mac_roman::from_str(str);
    std::vector<uint8_t> bytes = mac_roman.bytes();
    
    if (bytes.size() > 0xFF) {
        bytes.resize(0xFF);
    }
    auto size = static_cast<uint8_t>(bytes.size());
    
    m_data.push_back(static_cast<uint8_t>(size));
    m_data.insert(std::end(m_data), std::begin(bytes), std::end(bytes));
}

void rsrc::data::write_cstr(const std::string& str, size_t size)
{
    std::vector<uint8_t> bytes;
    
    if (size == 0) {
        // NUL Terminated C-String.
        auto mac_roman = rsrc::mac_roman::from_str(str);
        bytes = mac_roman.bytes();
        bytes.push_back(0);
    }
    else {
        // Fixed length C-String
        auto mac_roman = rsrc::mac_roman::from_str(str);
        bytes = mac_roman.bytes();
        bytes.resize(size, 0x00);
    }
    
    m_data.insert(std::end(m_data), std::begin(bytes), std::end(bytes));
}

void rsrc::data::write_data(const rsrc::data data)
{
    write_data(data.m_data);
}

void rsrc::data::write_data(const std::vector<uint8_t> bytes)
{
    if (m_ptr >= size()) {
        m_data.insert(m_data.end(), bytes.begin(), bytes.end());
        m_ptr += bytes.size();
    }
    else {
        // TODO: Handle inserting data in the middle of the data stream
        throw std::runtime_error("not implemented");
    }
}

void rsrc::data::pad_to_size(int64_t size)
{
    while (m_ptr < size) {
        write_byte(0x00);
    }
}

// MARK: - Accessors

uint64_t rsrc::data::size() const
{
    return static_cast<uint64_t>(m_data.size());
}
