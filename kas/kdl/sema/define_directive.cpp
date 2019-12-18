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

#include <iostream>
#include <stdexcept>
#include "kdl/sema/define_directive.hpp"
#include "diagnostic/log.hpp"
#include "assemblers/assembler.hpp"

// MARK: - Private Parser Functions

static inline std::string parse_constant_item(kdl::sema *sema)
{
    // Specify the name of the directive.
    sema->ensure({ kdl::condition(kdl::lexer::token::type::equals).truthy() });
    
    // Get the value of the item.
    if (sema->expect({ kdl::condition(kdl::lexer::token::type::string).falsey() })) {
        log::error(sema->peek().file(), sema->peek().line(), "Type definition constant must be a string.");
    }
    return sema->read().text();
}

static inline std::string parse_field_name(kdl::sema *sema)
{
    sema->ensure({ kdl::condition(kdl::lexer::token::type::lparen).truthy() });
    
    if (sema->expect({ kdl::condition(kdl::lexer::token::type::string).falsey() })) {
        log::error(sema->peek().file(), sema->peek().line(), "Type definition field name should be a string.");
    }
    auto field_name = sema->read().text();
    
    sema->ensure({ kdl::condition(kdl::lexer::token::type::rparen).truthy() });
    
    return field_name;
}

static inline kdk::assembler::field::value parse_field_value(kdl::sema *sema)
{
    auto file = sema->peek().file();
    auto line = sema->peek().line();
    
    sema->ensure({ kdl::condition(kdl::lexer::token::type::lparen).truthy() });
    
    // Default attributes - all attributes are provided as strings ready to be parsed
    // later
    kdk::assembler::field::value::type value_type { kdk::assembler::field::value::type::integer };
    uint64_t value_size { 0 };
    uint64_t value_length { 0 };
    std::string value_name;
    uint64_t value_offset { 0 };
    
    bool length_required = false;
    bool size_required = false;
    
    while (sema->expect({ kdl::condition(kdl::lexer::token::type::rparen).falsey() })) {
        
        if (sema->expect({ kdl::condition(kdl::lexer::token::type::identifier).falsey(), kdl::condition(kdl::lexer::token::type::equals).falsey() })) {
            log::error(sema->peek().file(), sema->peek().line(), "Malformed value attribute encountered.");
        }
        auto attribute = sema->read().text();
        sema->advance();
        
        if (attribute == "name") {
            if (sema->expect({ kdl::condition(kdl::lexer::token::type::string).falsey() })) {
                log::error(sema->peek().file(), sema->peek().line(), "The name attribute of a type definition value must be a string.");
            }
            value_name = sema->read().text();
        }
        else if (attribute == "offset") {
            if (sema->expect({ kdl::condition(kdl::lexer::token::type::integer).falsey() })) {
                log::error(sema->peek().file(), sema->peek().line(), "The offset attribute of a type definition value must be an integer.");
            }
            value_offset = std::stoull(sema->read().text());
        }
        else if (attribute == "length") {
            if (sema->expect({ kdl::condition(kdl::lexer::token::type::integer).falsey() })) {
                log::error(sema->peek().file(), sema->peek().line(), "The length attribute of a type definition value must be an integer.");
            }
            value_length = std::stoull(sema->read().text());
        }
        else if (attribute == "size") {
            if (sema->expect({ kdl::condition(kdl::lexer::token::type::integer).truthy() })) {
                value_size = std::stoull(sema->read().text());
            }
            else if (sema->expect({ kdl::condition(kdl::lexer::token::type::identifier).truthy() })) {
                auto size_symbol = sema->read().text();
                
                if (size_symbol == "byte") {
                    value_size = 1;
                }
                else if (size_symbol == "word") {
                    value_size = 2;
                }
                else if (size_symbol == "dword" || size_symbol == "long") {
                    value_size = 4;
                }
                else if (size_symbol == "qword" || size_symbol == "quad") {
                    value_size = 8;
                }
                else {
                    log::error(sema->peek().file(), sema->peek().line(), "Unrecognised size type '" + size_symbol + "'.");
                }
                
            }
            else {
                log::error(sema->peek().file(), sema->peek().line(), "The offset attribute of a type definition value must be an integer.");
            }
        }
        else if (attribute == "type") {
            if (sema->expect({ kdl::condition(kdl::lexer::token::type::identifier).falsey() })) {
                log::error(sema->peek().file(), sema->peek().line(), "The type attribute of a type definition value must be an identifier.");
            }
            
            auto type_symbol = sema->read().text();
            
            if (type_symbol == "resource_reference") {
                value_size = 2;
                value_type = kdk::assembler::field::value::type::resource_reference;
            }
            else if (type_symbol == "integer") {
                size_required = true;
                value_type = kdk::assembler::field::value::type::integer;
            }
            else if (type_symbol == "string") {
                length_required = true;
                value_type = kdk::assembler::field::value::type::string;
            }
            else if (type_symbol == "c_string") {
                value_type = kdk::assembler::field::value::type::c_string;
            }
            else if (type_symbol == "resource_reference") {
                value_type = kdk::assembler::field::value::type::p_string;
            }
            else if (type_symbol == "color") {
                value_size = 4;
                value_type = kdk::assembler::field::value::type::color;
            }
            else if (type_symbol == "bitmask") {
                size_required = true;
                value_type = kdk::assembler::field::value::type::resource_reference;
            }
            else {
                log::error(sema->peek().file(), sema->peek().line(), "Unrecognised type '" + type_symbol + "'.");
            }
        }
        else {
            // Unrecognised attribute.
            log::error(sema->peek().file(), sema->peek().line(), "Unrecognised value attribute '" + attribute + "' encountered.");
        }
        
        // Check for a comma. If no comma exists, then we require the presence of a rparen.
        if (sema->expect({ kdl::condition(kdl::lexer::token::type::comma).truthy() })) {
            sema->advance();
            continue;
        }
        
        break;
    }
    
    sema->ensure({ kdl::condition(kdl::lexer::token::type::rparen).truthy() });
    
    // Construct the value structure.
    if (size_required && value_size == 0) {
        log::error(file, line, "Expected the 'size' attribute to be specified on type definition field value.");
    }
    
    if (length_required && value_length == 0) {
        log::error(file, line, "Expected the 'length' attribute to be specified on type definition field value.");
    }
    
    return kdk::assembler::field::value(value_name, value_type, value_offset, length_required ? value_length : value_size);
}

// MARK: - Parser

void kdl::define_directive::parse(kdl::sema *sema)
{
    std::string resource_type_name;
    std::string resource_type_code;
    std::vector<kdk::assembler::field> resource_fields;
    
    auto file = sema->peek().file();
    auto line = sema->peek().line();
    
    // Keep going until we encounter the closing brace.
    while (sema->expect({ kdl::condition(kdl::lexer::token::type::rbrace).falsey() })) {
        
        // All items in the directive start with an identifier. Check what the identifier
        // is in order to determine the course of action.
        auto item_name = sema->read().text();
        
        if (item_name == "name") {
            resource_type_name = parse_constant_item(sema);
        }
        else if (item_name == "code") {
            resource_type_code = parse_constant_item(sema);
        }
        else if (item_name == "field") {
            // Add a new field into the resource type.
            // The syntax is:
            //  field(field-name) { args }
            
            auto field_name = parse_field_name(sema);
            auto required = false;
            std::string deprecated;
            std::vector<kdk::assembler::field::value> field_values;
            
            sema->ensure({
                kdl::condition(kdl::lexer::token::type::lbrace).truthy()
            });
            
            // Loop until we find the terminating r-brace.
            while (sema->expect({ kdl::condition(kdl::lexer::token::type::rbrace).falsey() })) {
                // All field attributes start with an identifier.
                if (sema->expect({ kdl::condition(kdl::lexer::token::type::identifier).falsey() })) {
                    log::error(sema->peek().file(), sema->peek().line(), "Type definition field attribute should start with an identifier");
                }
                auto attribute_name = sema->read().text();
                
                if (attribute_name == "required") {
                    required = true;
                }
                else if (attribute_name == "value") {
                    auto value = parse_field_value(sema);
                    
                    field_values.push_back(value);
                }
                
                sema->ensure({ kdl::condition(kdl::lexer::token::type::semi_colon).truthy() });
            }
            
            sema->ensure({
                kdl::condition(kdl::lexer::token::type::rbrace).truthy()
            });
            
            // Construct the field.
            resource_fields.push_back(
                kdk::assembler::field(field_name)
                    .set_values(field_values)
                	.set_required(required)
			);
            
        }
        
        sema->ensure({ kdl::condition(kdl::lexer::token::type::semi_colon).truthy() });
    }
    
    // Validate the type being defined.
    if (resource_type_code.empty()) {
        log::error(file, line, "Type definition must include a type code.");
    }
    
    if (resource_type_name.empty()) {
        log::error(file, line, "Type definition must include a type name.");
    }
    
    if (resource_fields.empty()) {
        log::error(file, line, "Type definition must include at least one field.");
    }
    
    std::cout << "Define resource type: " << resource_type_name << " '" << resource_type_code << "'" << std::endl;
}
