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
#include "kdl/lexer.hpp"
#include "kdl/sema.hpp"

#if !defined(KDL_DIRECTIVE)
#define KDL_DIRECTIVE

namespace kdl
{

/**
 * Directive's are commands that tell the assembler to do something immediately,
 * or alter its state.
 */
struct directive
{
public:
    /**
     * Check for the presence of a directive.
     */
    static bool test(kdl::sema *sema);
    
    /**
     * Parse a directive.
     *
     * This parses the token stream in sema at the point of a directive being
     * found. Attempting to call this when the token stream is not sat on a
     * directive token will result in an error.
     */
    static void parse(kdl::sema *sema);
};

};

#endif
