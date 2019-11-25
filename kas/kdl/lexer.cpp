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
