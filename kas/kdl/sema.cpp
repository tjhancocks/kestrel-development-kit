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

#include "kdl/sema.hpp"
#include <iostream>
#include "kdl/sema/directive.hpp"

// MARK: - Constructor

kdl::sema::sema(const std::vector<kdl::lexer::token> tokens)
    : m_tokens(tokens), m_ptr(0)
{
    
}

// MARK: - Semantic Analysis

void kdl::sema::run()
{
    // Reset the iterator, in the event of running the semantic analysis twice we
    // need to do this.
    m_ptr = 0;
    
    while (!finished()) {
        
        if (kdl::directive::test(this)) {
            kdl::directive::parse(this);
        }
        
    }
}

// MARK: - Stream

bool kdl::sema::finished(long offset, long count) const
{
    auto ptr = (m_ptr + offset);
    auto end_ptr = ptr + count;
    auto size = m_tokens.size();
    return ptr > size || end_ptr > size;
}

void kdl::sema::advance(long delta)
{
    m_ptr += delta;
}

kdl::lexer::token kdl::sema::read(long offset)
{
    auto tk = peek(offset);
    m_ptr += offset + 1;
    return tk;
}

kdl::lexer::token kdl::sema::peek(long offset) const
{
    if (finished(offset, 1)) {
        throw std::runtime_error("Attempted to access token beyond end of token stream.");
    }
    
    return m_tokens[m_ptr + offset];
}

std::vector<kdl::lexer::token> kdl::sema::consume(kdl::condition::falsey_function f)
{
    std::vector<kdl::lexer::token> v;
    
    while (!finished() && f(peek())) {
        v.push_back(read());
    }
    
    return v;
}


// MARK: - Conditions / Expectations

bool kdl::sema::expect(kdl::condition::truthy_function f) const
{
    return !finished() && f(peek());
}

template <typename ...T>
bool kdl::sema::expect(kdl::condition::truthy_function f, T... fArgs) const
{
    auto ptr = 0;
    std::vector<kdl::condition::truthy_function> v = { f, fArgs... };
    for (decltype(v)::iterator i = v.begin(); i != v.end() && !finished(ptr); ++i, ++ptr) {
        if ((*i)(peek(ptr)) == false) {
            return false;
        }
    }
    return true;
}

kdl::condition::condition(kdl::lexer::token::type Ty)
    : m_Ty(Ty), m_Tx("")
{
    
}

kdl::condition::condition(const std::string Tx)
    : m_Ty(kdl::lexer::token::type::unknown), m_Tx(Tx)
{
    
}

kdl::condition::condition(kdl::lexer::token::type Ty, const std::string Tx)
    : m_Ty(Ty), m_Tx(Tx)
{
    
}

kdl::condition::evaluation_function kdl::condition::to_be(bool r)
{
    auto& Tx = m_Tx;
    auto& Ty = m_Ty;
    
    return [Tx, Ty, r] (kdl::lexer::token Tk) -> bool {
        bool outcome = true;
        
        if (!Tx.empty() && Tx != Tk.text()) {
            outcome = false;
        }
        
        if (Ty != kdl::lexer::token::type::unknown && !Tk.is_a(Ty)) {
            outcome =  false;
        }
        
        return (outcome == r);
    };
}

kdl::condition::truthy_function kdl::condition::truthy()
{
    return to_be(true);
}

kdl::condition::falsey_function kdl::condition::falsey()
{
    return to_be(false);
}
