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

#include <tuple>
#include "types/base_type.hpp"

// MARK: - Constructor

kdk::base_type::base_type(kdk::resource res, std::shared_ptr<rsrc::file> rf)
    : m_source(res), m_rf(rf)
{
    
}

// MARK: -

rsrc::data kdk::base_type::assemble()
{
    throw std::runtime_error("Must be overriden in subclass.");
}

// MARK: - Field Access

bool kdk::base_type::field_exists(const std::string name, std::initializer_list<kdk::resource::field::value_type> allowed_types) const
{
    auto f = m_source.field_named(name);
    if (!f) {
        return false;
    }
    
    if (allowed_types.size() > 0) {
        auto values = f->values();
        if (allowed_types.size() != values.size()) {
            return false;
        }
        
        auto n = 0;
        for (auto allowed : allowed_types) {
            if (allowed != std::get<1>(values[n++])) {
                return false;
            }
        }
        
        // Reaching here indicates that all values matched.
        return true;
    }
    
    return true;
}

int64_t kdk::base_type::resource_id_for(const std::string name, const int value_index) const
{
    auto f = m_source.field_named(name);
    if (!f) {
        return 0;
    }
    
    auto value = std::get<0>(f->values().at(value_index));
    return std::stoll(value);
}

int64_t kdk::base_type::import_and_resolve_file(const std::string name, const int value_index) const
{
    return 0;
}
