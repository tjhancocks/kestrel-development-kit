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

#if !defined(KDL_DECLARATION)
#define KDL_DECLARATION

namespace kdl
{

/**
 * Declarations are how resource types are represented in KDL. A declaration specifies the
 * type of resource being worked on, and a number of instantiations of that resource.
 *
 *  declare ResourceType { instantiations... }
 *
 */
struct declaration
{
public:
    /**
     * Check for the presence of a declaration.
     */
    static bool test(kdl::sema *sema);
    
    /**
     * Parse a declaration.
     *
     * This parses the token stream in sema at the point of a declaration being
     * found. Attempting to call this when the token stream is not sat on a
     * declaration token will result in an error.
     */
    static void parse(kdl::sema *sema);
    
private:
    static void parse_instance(kdl::sema *sema);
};

};

#endif
