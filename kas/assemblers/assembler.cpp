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

#include <fstream>
#include <tuple>
#include "assemblers/assembler.hpp"

// MARK: - Helper Functions

namespace std
{

bool has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

};

// MARK: - Constructor

kdk::assembler::assembler(kdk::resource res, std::shared_ptr<rsrc::file> rf)
    : m_source(res), m_rf(rf)
{
    
}

// MARK: -

rsrc::data kdk::assembler::assemble()
{
    throw std::runtime_error("Must be overriden in subclass.");
}

// MARK: - Field Access

bool kdk::assembler::field_exists(const std::string name, std::initializer_list<kdk::resource::field::value_type> allowed_types) const
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

int64_t kdk::assembler::resource_id_for(const std::string name, const int value_index) const
{
    auto f = m_source.field_named(name);
    if (!f) {
        return 0;
    }
    
    auto value = std::get<0>(f->values().at(value_index));
    return std::stoll(value);
}

int64_t kdk::assembler::integer_for(const std::string name, const int value_index) const
{
    auto f = m_source.field_named(name);
    if (!f) {
        return 0;
    }
    
    auto value = std::get<0>(f->values().at(value_index));
    return std::stoll(value);
}

std::string kdk::assembler::string_for(const std::string name, const int value_index) const
{
    auto f = m_source.field_named(name);
    if (!f) {
        return 0;
    }
    
    return std::get<0>(f->values().at(value_index));
}

std::shared_ptr<rsrc::file::resource> kdk::assembler::import_and_resolve_file(const std::string name, int64_t resource_id, const int value_index) const
{
    // Load the contents of the file as binary data.
    auto path = string_for(name, value_index);
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open() || file.fail()) {
        throw std::runtime_error("Failed to import file: " + path);
    }
    
    // Make sure we don't skip newlines.
    file.unsetf(std::ios::skipws);
    
    // Get the size of the file.
    file.seekg(0UL, std::ios::end);
    auto file_size = file.tellg();
    file.seekg(0UL, std::ios::beg);
    
    // Read the contents of the file into the vector.
    auto data = std::make_shared<std::vector<uint8_t>>(file_size);
    data->insert(data->begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());
    
    // Close the file and clean up.
    file.close();
    
    // Determine what the type of data is. For this we'll just used the file extension.
    if (std::has_suffix(path, ".tga")) {
        // TäRG Resource
        return m_rf->add_resource("TäRG", resource_id, "", rsrc::data(data));
    }
    else if (std::has_suffix(path, ".png")) {
        // PïNG Resource
        return m_rf->add_resource("PïNG", resource_id, "", rsrc::data(data));
    }
    
    // At this point we assume we couldn't determine the type
    return nullptr;
}
