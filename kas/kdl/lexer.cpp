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
#include <algorithm>
#include <fstream>
#include <streambuf>
#include <iostream>

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
    : m_source(content + "\n"), m_pos(0), m_length(content.length() + 1)
{
    
}

kdl::lexer kdl::lexer::open_file(const std::string path)
{
    std::ifstream f(path);
    std::string str;

    f.seekg(0, std::ios::end);
    str.reserve(f.tellg());
    f.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(f)),
                std::istreambuf_iterator<char>());
    
    return kdl::lexer(str);
}

// MARK: - Lexical Analysis

std::vector<kdl::lexer::token> kdl::lexer::analyze()
{
    while (available()) {
        
        // Check if we're looking at a new line character. If we are then simply consume it, and
        // increment the current line number.
        if (test_if(match<'\n'>::yes)) {
            advance();
            m_line++;
            continue;
        }
        
        // Consume any leading (nonbreaking) whitespace.
        consume_while(set<' ', '\t'>::contains);
        
        // Check for a comment. If we're looking at a comment then we need to consume the entire
        // line. We need to advance past the character at the end of the match.
        if (test_if(match<';'>::yes)) {
            consume_while(match<'\n'>::no);
        }
        
        // Constructs
        else if (test_if(match<'@'>::yes)) {
            // We're looking at a directive.
            // Directive's are defined in the form of `@name`, an '@' followed by an identifier.
            advance();
            consume_while(identifier_set::contains);
            
            m_tokens.push_back(kdl::lexer::token(m_line, 0, m_slice, token::type::directive));
        }
        
        // Literals
        else if (test_if(match<'"'>::yes)) {
            // We're looking at a string literal.
            // The string continues until a corresponding '"' is found.
            advance();
            consume_while(match<'"'>::no);
            m_tokens.push_back(kdl::lexer::token(m_line, 0, m_slice, token::type::string));
            advance();
        }
        else if (test_if(match<'#'>::yes)) {
            // We're looking at the beginning of a resource id literal.
            // These take the form of #128, #129, etc.
            advance();
            consume_while(number_set::contains);
            m_tokens.push_back(kdl::lexer::token(m_line, 0, m_slice, token::type::resource_id));
        }
        else if (test_if(number_set::contains)) {
            // We're looking at a number, slice it out of the source, and then check the following
            // character.
            consume_while(number_set::contains);
            auto number_text = m_slice;
            
            if (test_if(match<'%'>::yes)) {
                // This is a percentage.
                advance();
                m_tokens.push_back(kdl::lexer::token(m_line, 0, number_text, token::type::percentage));
            }
            else {
                m_tokens.push_back(kdl::lexer::token(m_line, 0, number_text, token::type::integer));
            }
        }
        else if (test_if(identifier_set::contains)) {
            // We're looking at an identifier. Extract the identifier and determine if it is a keyword.
            consume_while(identifier_set::contains);
            auto text = m_slice;
            
            // TODO: Check for keywords
            
            m_tokens.push_back(kdl::lexer::token(m_line, 0, text, token::type::identifier));
        }
        
        // Symbols
        else if (test_if(match<'{'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::lbrace));
        }
        else if (test_if(match<'}'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::rbrace));
        }
        else if (test_if(match<'['>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::lbracket));
        }
        else if (test_if(match<']'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::rbracket));
        }
        else if (test_if(match<'('>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::lparen));
        }
        else if (test_if(match<')'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::rparen));
        }
        else if (test_if(match<'<'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::langle));
        }
        else if (test_if(match<'>'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::rangle));
        }
        else if (test_if(match<'='>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::equals));
        }
        else if (test_if(match<'+'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::plus));
        }
        else if (test_if(match<'-'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::minus));
        }
        else if (test_if(match<'*'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::star));
        }
        else if (test_if(match<'/'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::slash));
        }
        else if (test_if(match<':'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::colon));
        }
        else if (test_if(match<','>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::comma));
        }
        else if (test_if(match<'.'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::dot));
        }
        else if (test_if(match<'&'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::ampersand));
        }
        else if (test_if(match<'|'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::pipe));
        }
        else if (test_if(match<'^'>::yes)) {
            m_tokens.push_back(kdl::lexer::token(m_line, 0, read(), token::type::caret));
        }
        
        // Error States
        else {
            throw std::runtime_error("Unrecognised character '" + peek() + "' encountered.");
        }
    }
    
    return m_tokens;
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

bool kdl::lexer::consume_while(std::function<bool(const std::string)> testFn)
{
    m_slice = "";
    while (testFn(peek())) {
        m_slice += read();
    }
    return !m_slice.empty();
}

template <char c>
bool kdl::match<c>::yes(const std::string __Chk)
{
    return __Chk == std::string(1, c);
}

template <char c>
bool kdl::match<c>::no(const std::string __Chk)
{
    return !yes(__Chk);
}

template <char lc, char uc>
bool kdl::range<lc, uc>::contains(const std::string __Chk)
{
    for (auto __ch : __Chk) {
        if (__ch < lc || __ch > uc) {
            return false;
        }
    }
    return true;
}

template <char lc, char uc>
bool kdl::range<lc, uc>::not_contains(const std::string __Chk)
{
    return !contains(__Chk);
}

template<char tC, char... ttC>
bool kdl::set<tC, ttC...>::contains(const std::string __Chk)
{
    std::vector<char> v = {tC, ttC...};
    
    for (auto __ch : __Chk) {
        if (std::find(v.begin(), v.end(), __ch) == v.end()) {
            return false;
        }
    }
    
    return true;
}

template<char tC, char... ttC>
bool kdl::set<tC, ttC...>::not_contains(const std::string __Chk)
{
    return !contains(__Chk);
}

bool kdl::identifier_set::contains(const std::string __Chk)
{
    for (auto __ch : __Chk) {
        auto condition = (__ch >= 'A' && __ch <= 'Z') || (__ch >= 'a' && __ch <= 'z') || __ch == '_';
        if (!condition) {
            return false;
        }
    }
    return true;
}

bool kdl::number_set::contains(const std::string __Chk)
{
    for (auto __ch : __Chk) {
        auto condition = (__ch >= '0' && __ch <= '9') || __ch == '_' || __ch == ',';
        if (!condition) {
            return false;
        }
    }
    return true;
}
