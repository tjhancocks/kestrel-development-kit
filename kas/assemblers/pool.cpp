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

#include "assemblers/pool.hpp"
#include "diagnostic/log.hpp"

// MARK: - Singleton

kdk::assembler_pool::assembler_pool()
{
    
}

kdk::assembler_pool& kdk::assembler_pool::shared()
{
    static kdk::assembler_pool instance;
    return instance;
}

// MARK: - Assembler Look-up

std::shared_ptr<kdk::assembler> kdk::assembler_pool::assembler_named(const std::string type_name, bool no_error) const
{
    for (auto t : m_assemblers) {
        if (std::get<0>(t) == type_name) {
            return std::get<2>(t);
        }
    }
    
    if (!no_error) {
        log::error("<missing>", 0, "Unrecognised declaration type '" + type_name + "'");
    }
    
    return nullptr;
}

std::string kdk::assembler_pool::type_code_named(const std::string type_name, bool no_error) const
{
    for (auto t : m_assemblers) {
        if (std::get<0>(t) == type_name) {
            return std::get<1>(t);
        }
    }
    
    if (!no_error) {
        log::error("<missing>", 0, "Unrecognised declaration type '" + type_name + "'");
    }
    
    return nullptr;
}


// MARK: - Assembler Registration

void kdk::assembler_pool::register_assembler(const std::string type_name, const std::string type_code, std::shared_ptr<kdk::assembler> assembler)
{
    // Ensure this is a unique/novel assembler.
    if (assembler_named(type_name, true)) {
        log::error("<missing>", 0, "Duplicated declaration type '" + type_name + "'");
    }
    
    if (type_code_named(type_name, true) == type_code) {
        log::error("<missing>", 0, "Duplicated resource type '" + type_code + "'");
    }
    
    m_assemblers.push_back(std::make_tuple(type_name, type_code, assembler));
}
