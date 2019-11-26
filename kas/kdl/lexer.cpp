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

#include "kdl/lexer.hpp"
#include <stdexcept>

// MARK: - Token

kdl::lexer::token::token()
    : m_line(0), m_offset(0), m_text("-"), m_type(unknown)
{
    
}

kdl::lexer::token::token(const int line, const int offset, const std::string text, kdl::lexer::token::type type)
    : m_line(line), m_offset(offset), m_text(text), m_type(type)
{
    
}

// MARK: - Token Accessors

int kdl::lexer::token::line() const
{
    return m_line;
}

int kdl::lexer::token::offset() const
{
    return m_offset;
}

std::string kdl::lexer::token::text() const
{
    return m_text;
}

// MARK: - Token Operations

bool kdl::lexer::token::is_a(kdl::lexer::token::type type) const
{
    return (m_type == type);
}

// MARK: - Lexer Constructor

kdl::lexer::lexer(const std::string& content)
    : m_source(content), m_pos(0), m_length(content.length())
{
    
}

// MARK: - Lexical Analysis

void kdl::lexer::analyze()
{
    while (available()) {
        
        advance();
    }
}

// MARK: - Lexer Accessors

bool kdl::lexer::available(long offset, std::string::size_type size) const
{
    auto start = m_pos + offset;
    auto end = start + size;
    return (end <= m_length);
}


// MARK: - Lexer Operations

void kdl::lexer::advance(long offset)
{
    m_pos += offset;
}

std::string kdl::lexer::peek(long offset, std::string::size_type size) const
{
    if (!available(offset, size)) {
        throw std::runtime_error("Failed to peek source.");
    }
    
    return m_source.substr(m_pos + offset, size);
}

std::string kdl::lexer::read(long offset, std::string::size_type size)
{
    auto s = peek(offset, size);
    advance(offset + size);
    return s;
}

// MARK: - Test / Validation

bool kdl::lexer::test_if(std::function<bool(const std::string)> testFn, long offset, std::string::size_type size) const
{
    auto chunk = peek(offset, size);
    return testFn(chunk);
}

template <char c>
bool kdl::match<c>::function(const std::string __Chk)
{
    return __Chk == std::string(1, c);
}

template <char lc, char uc>
bool kdl::in_range<lc, uc>::function(const std::string __Chk)
{
    for (auto __ch : __Chk) {
        if (__ch < lc || __ch > uc) {
            return false;
        }
    }
    return true;
}

template <char c>
bool kdl::not_match<c>::function(const std::string __Chk)
{
    return !kdl::match<c>::function(__Chk);
}
