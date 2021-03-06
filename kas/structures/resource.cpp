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

#include "structures/resource.hpp"

// MARK: - Constructor

kdk::resource::resource(const std::string type, const int64_t id, const std::string name)
    : m_type(type), m_id(id), m_name(name)
{
    
}

// MARK: - Field

kdk::resource::field::field(const std::string name, std::vector<std::tuple<std::string, value_type>> values)
    : m_name(name), m_values(values)
{
    
}

std::string kdk::resource::field::name() const
{
    return m_name;
}

std::vector<std::tuple<std::string, kdk::resource::field::value_type>> kdk::resource::field::values() const
{
    return m_values;
}

std::shared_ptr<kdk::resource::field> kdk::resource::field_named(const std::string name, bool required) const
{
    for (auto f : m_fields) {
        if (f.name() == name) {
            return std::make_shared<kdk::resource::field>(f);
        }
    }
    
    if (required) {
        throw std::runtime_error("Resource #" + std::to_string(m_id) + " '" + m_type + "' was missing field '" + name + "'");
    }
    
    return nullptr;
}

// MARK: - Accessors

int64_t kdk::resource::id() const
{
    return m_id;
}

std::string kdk::resource::name() const
{
    return m_name;
}

std::string kdk::resource::type() const
{
    return m_type;
}

// MARK: - Mutators

void kdk::resource::add_field(const kdk::resource::field& field)
{
    m_fields.push_back(field);
}
