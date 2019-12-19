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

#include <memory>
#include <vector>
#include <tuple>
#include "assemblers/assembler.hpp"

#if !defined(KDK_ASSEMBLER_POOL)
#define KDK_ASSEMBLER_POOL

namespace kdk
{

/**
 * The Assembler Pool is responsible for storing and keeping track of
 * assemblers that may have been created in KDL, or KAS itself.
 *
 * When an assembler is requested that a pointer to the assembler will be
 * returned for use.
 */
class assembler_pool
{
public:
    assembler_pool(const assembler_pool&) = delete;
    assembler_pool& operator=(const assembler_pool &) = delete;
    assembler_pool(assembler_pool &&) = delete;
    assembler_pool & operator=(assembler_pool &&) = delete;
    
    static assembler_pool& shared();
    
    std::tuple<std::string, std::shared_ptr<kdk::assembler>> assembler_named(const std::string type_name, bool no_error = false) const;
    void register_assembler(const std::string type_name, const std::string type_code, std::shared_ptr<kdk::assembler> assembler);
    
private:
    std::vector<std::tuple<std::string, std::string, std::shared_ptr<kdk::assembler>>> m_assemblers;
    assembler_pool();
    
};

};

#endif
