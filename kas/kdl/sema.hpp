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

#include <vector>
#include <string>
#include <initializer_list>
#include "kdl/lexer.hpp"
#include "structures/target.hpp"

#if !defined(KDL_SEMA)
#define KDL_SEMA

namespace kdl
{

// MARK: - Expectation Condition Templates

struct condition {
public:
    typedef std::function<bool(kdl::lexer::token)> evaluation_function;
    typedef std::function<bool(kdl::lexer::token)> truthy_function;
    typedef std::function<bool(kdl::lexer::token)> falsey_function;
    
    condition(kdl::lexer::token::type Ty);
    condition(const std::string Tx);
    condition(kdl::lexer::token::type Ty, const std::string Tx);
    
    evaluation_function to_be(bool r);
    
    truthy_function truthy();
    falsey_function falsey();
    
private:
    kdl::lexer::token::type m_Ty;
    std::string m_Tx;
};

// MARK: - Semantic Analysis

/**
 * The `kdl::sema` class is responsible for performing semantic analysis on a
 * token strem produced by `kdl::lexer` and inferring meaning, constructing
 * objects based upon it.
 */
class sema
{
public:
    
    /**
     * Construct a new `kdl::sema` instance using the specified token stream.
     */
    sema(std::shared_ptr<kdk::target> target, const std::vector<kdl::lexer::token> tokens);
    
    /**
     * Run/perform semantic analysis on the token stream.
     */
    void run();
    
    /**
     * Check if the semantic analysis has reached the end of the token stream.
     */
    bool finished(long offset = 0, long count = 1) const;
   
    /**
     * Keep consuming tokens until the expectation is met.
     *
     * \return A vector containing all the tokens that were consumed.
     */
    std::vector<kdl::lexer::token> consume(kdl::condition::falsey_function f);
    
    /**
     * Advance past the specified number of tokens.
     */
    void advance(long delta = 1);
    
    /**
     * Read a token from the token stream.
     */
    kdl::lexer::token read(long offset = 0);
    
    /**
     * Peek a token from the token stream.
     */
    kdl::lexer::token peek(long offset = 0) const;
    
    /**
     * Validate the expectation of a token.
     */
    bool expect(kdl::condition::truthy_function f) const;
    
    /**
     * Validate the expection of a sequence of tokens.
     */
    bool expect(std::initializer_list<kdl::condition::truthy_function> f) const;
    
    /**
     * Ensure the next sequence of tokens matches exactly what is specified.
     * Each token is advanced past.
     */
     bool ensure(std::initializer_list<kdl::condition::truthy_function> f);
    
    /**
     * Returns a reference to the current target.
     */
    std::shared_ptr<kdk::target> target();
    
    /**
     * Insert new tokens into the token stream at the current location.
     */
    void insert_tokens(std::vector<kdl::lexer::token> tokens);
    
private:
    long m_ptr { 0 };
    std::vector<kdl::lexer::token> m_tokens;
    std::shared_ptr<kdk::target> m_target;
};


};

#endif
