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
#include "kdl/lexer.hpp"

#if !defined(KDL_SEMA)
#define KDL_SEMA

namespace kdl
{

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
    sema(const std::vector<kdl::lexer::token> tokens);
    
private:
    std::vector<kdl::lexer::token> m_tokens;
};

};

#endif
