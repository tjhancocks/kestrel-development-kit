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

#include "assemblers/assembler.hpp"
#include "diagnostic/log.hpp"

// MARK: - Constructor

kdk::assembler::assembler(const kdk::resource& resource)
    : m_resource(resource)
{
    
}

// MARK: - Assembly

rsrc::data kdk::assembler::assemble()
{
    return m_blob;
}

void kdk::assembler::assemble(kdk::assembler::field field)
{
    // Find the field with in the resource
    auto resource_field = find_field(field.name(), field.is_required());
    
    // Ensure the data object is large enough for this field.
    m_blob.set_insertion_point(m_blob.size());
    m_blob.pad_to_size(field.required_data_size());
    m_blob.set_insertion_point(field.offset());
    
    // Is the field deprecated? If show show a warning.
    if (field.is_deprecated()) {
        log::warning("<missing>", 0, "The field '" + field.name() + "' is deprecated.");
    }
    
    // If the field was provided in the script, then handle it, otherwise try to fill it in with
    // default values.
    if (resource_field) {
        // Check the number of values matches what we actually have.
        if (resource_field->values().size() != field.expected_values().size()) {
            log::error("<missing>", 0, "Incorrect number of values passed to field '" + field.name() + "'.");
        }
        
        // Prepare to encode and validate each of the values.
        for (auto n = 0; n < field.expected_values().size(); ++n) {
            auto value = resource_field->values()[n];
            auto expected_value = field.expected_values()[n];
            
            if (!expected_value.type_allowed(std::get<1>(value))) {
                // The value type is incorrect
                log::error("<missing>", 0, "Incorrect value type provided on field '" + field.name() + "' value " + std::to_string(n) + ".");
            }
            
            // Seek to the appropriate location in the data for encoding.
            m_blob.set_insertion_point(expected_value.offset());
            
            // Handle the value appropriately and encode it into the data.
            switch (std::get<1>(value)) {
                case kdk::resource::field::value_type::integer:
                case kdk::resource::field::value_type::percentage: {
                    switch (expected_value.size()) {
                        case 1:
                            m_blob.write_byte(static_cast<uint8_t>(std::stoi(std::get<0>(value))));
                            break;
                        case 2:
                            m_blob.write_word(static_cast<uint16_t>(std::stoi(std::get<0>(value))));
                            break;
                        case 4:
                            m_blob.write_long(static_cast<uint32_t>(std::stoi(std::get<0>(value))));
                            break;
                        case 8:
                            m_blob.write_quad(static_cast<uint64_t>(std::stoi(std::get<0>(value))));
                            break;
                        default:
                            throw std::runtime_error("Unexpected integer size expectation encountered.");
                    }
                }
                    
                case kdk::resource::field::value_type::resource_id: {
                    m_blob.write_signed_word(static_cast<int16_t>(std::stoi(std::get<0>(value))));
                    break;
                }
                    
                case kdk::resource::field::value_type::string: {
                    // TODO
                    break;
                }
                    
                case kdk::resource::field::value_type::identifier: {
                    // TODO
                    break;
                }
                    
                case kdk::resource::field::value_type::file_reference: {
                    // TODO
                    break;
                }
            }
        }
    }
    else {
        // No field was specified in the resource, so write the default values.
        for (auto expected : field.expected_values()) {
            expected.write_default_value(m_blob);
        }
    }
    
}

// MARK: - Fields

kdk::assembler::field::field(const std::string& name)
    : m_name(name)
{
    
}

kdk::assembler::field kdk::assembler::field::named(const std::string &name)
{
    return kdk::assembler::field(name);
}

kdk::assembler::field kdk::assembler::field::set_deprecated(bool deprecated)
{
    m_deprecated = deprecated;
    return *this;
}

kdk::assembler::field kdk::assembler::field::set_required(bool required)
{
    m_required = required;
    return *this;
}

kdk::assembler::field kdk::assembler::field::set_values(const std::vector<kdk::assembler::field::value>& values)
{
    m_expected_values = values;
    return *this;
}

uint64_t kdk::assembler::field::size() const
{
    uint64_t size = 0;
    for (auto v : m_expected_values) {
        size += v.size();
    }
    return size;
}

uint64_t kdk::assembler::field::required_data_size() const
{
    uint64_t minimum_size = 0;
    for (auto v : m_expected_values) {
        uint64_t size = v.offset() + v.size();
        minimum_size = std::max(minimum_size, size);
    }
    return minimum_size;
}

uint64_t kdk::assembler::field::offset() const
{
    return m_expected_values[0].offset();
}

bool kdk::assembler::field::is_required() const
{
    return m_required;
}

bool kdk::assembler::field::is_deprecated() const
{
    return m_deprecated;
}

std::string& kdk::assembler::field::name()
{
    return m_name;
}

std::vector<kdk::assembler::field::value>& kdk::assembler::field::expected_values()
{
    return m_expected_values;
}

// MARK: - Values

kdk::assembler::field::value::value(std::string name, kdk::assembler::field::value::type type, uint64_t offset, uint64_t size)
    : m_name(name), m_type_mask(type), m_offset(offset), m_size(size)
{
    
}

kdk::assembler::field::value kdk::assembler::field::value::expect(const std::string& name, kdk::assembler::field::value::type type, uint64_t offset, uint64_t size)
{
    return kdk::assembler::field::value(name, type, offset, size);
}

kdk::assembler::field::value kdk::assembler::field::value::set_symbols(const std::vector<std::tuple<std::string, int64_t>> symbols)
{
    m_symbols = symbols;
    return *this;
}

kdk::assembler::field::value kdk::assembler::field::value::set_default_value(const std::function<void(rsrc::data&)> default_value)
{
    m_default_value = default_value;
    return *this;
}

uint64_t kdk::assembler::field::value::size() const
{
    return m_size;
}

uint64_t kdk::assembler::field::value::offset() const
{
    return m_offset;
}

bool kdk::assembler::field::value::type_allowed(kdk::resource::field::value_type type) const
{
    switch (type) {
        case kdk::resource::field::value_type::file_reference:
        case kdk::resource::field::value_type::resource_id: {
            return m_type_mask & kdk::assembler::field::value::type::resource_reference;
        }
        case kdk::resource::field::value_type::identifier: {
            if (!m_symbols.empty()) {
                return m_type_mask & kdk::assembler::field::value::type::resource_reference;
            }
            return m_type_mask & (kdk::assembler::field::value::type::integer | kdk::assembler::field::value::type::bitmask);
        }
        case kdk::resource::field::value_type::integer: {
            return m_type_mask & (kdk::assembler::field::value::type::integer | kdk::assembler::field::value::type::bitmask);
        }
        case kdk::resource::field::value_type::string: {
            return m_type_mask & kdk::assembler::field::value::type::string;
        }
        case kdk::resource::field::value_type::percentage: {
            return m_type_mask & kdk::assembler::field::value::type::integer;
        }
    }
}

void kdk::assembler::field::value::write_default_value(rsrc::data& data) const
{
    if (m_default_value) {
        data.set_insertion_point(m_offset);
        m_default_value(data);
    }
}

// MARK: - Field Functions

std::shared_ptr<kdk::resource::field> kdk::assembler::find_field(std::string& name, bool required) const
{
    auto field = m_resource.field_named(name);
    if (required && !field) {
        log::error("<missing>", 0, "Missing field '" + name + "' in resource.");
    }
    return field;
}
