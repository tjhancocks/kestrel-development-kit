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
#include "assemblers/pool.hpp"

// MARK: - Private Parser Functions

static inline kdk::assembler::field::value::type parse_value_type(kdl::sema *sema)
{
    if (sema->expect({ kdl::condition(kdl::lexer::token::type::identifier).falsey() })) {
        log::error(sema->peek().file(), sema->peek().line(), "The type attribute of a type definition value must be an identifier.");
    }
    
    auto type_symbol = sema->read().text();
    
    if (type_symbol == "resource_reference") {
        return kdk::assembler::field::value::type::resource_reference;
    }
    else if (type_symbol == "integer") {
        return kdk::assembler::field::value::type::integer;
    }
    else if (type_symbol == "string") {
        return kdk::assembler::field::value::type::string;
    }
    else if (type_symbol == "c_string") {
        return kdk::assembler::field::value::type::c_string;
    }
    else if (type_symbol == "p_string") {
        return kdk::assembler::field::value::type::p_string;
    }
    else if (type_symbol == "color") {
        return kdk::assembler::field::value::type::color;
    }
    else if (type_symbol == "bitmask") {
        return kdk::assembler::field::value::type::resource_reference;
    }
    else {
        log::error(sema->peek().file(), sema->peek().line(), "Unrecognised type '" + type_symbol + "'.");
    }
    
    throw std::runtime_error("Fatal error whilst resolving type symbol.");
}

static inline uint64_t parse_value_size(kdl::sema *sema)
{
    if (sema->expect({ kdl::condition(kdl::lexer::token::type::integer).truthy() })) {
        return std::stoull(sema->read().text());
    }
    else if (sema->expect({ kdl::condition(kdl::lexer::token::type::identifier).truthy() })) {
        auto size_symbol = sema->read().text();
        
        if (size_symbol == "byte") {
            return 1;
        }
        else if (size_symbol == "word") {
            return 2;
        }
        else if (size_symbol == "dword" || size_symbol == "long") {
            return 4;
        }
        else if (size_symbol == "qword" || size_symbol == "quad") {
            return 8;
        }
        else {
            log::error(sema->peek().file(), sema->peek().line(), "Unrecognised size type '" + size_symbol + "'.");
        }
    }
    else {
        log::error(sema->peek().file(), sema->peek().line(), "The offset attribute of a type definition value must be an integer.");
    }
    
    throw std::runtime_error("Fatal error whilst resolving value size.");
}

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
            value_size = parse_value_size(sema);
        }
        else if (attribute == "type") {
            value_type = parse_value_type(sema);
            
            switch (value_type) {
                case kdk::assembler::field::value::type::resource_reference:
                    value_size = 2;
                    break;
                    
                case kdk::assembler::field::value::type::integer:
                    size_required = true;
                    break;
                    
                case kdk::assembler::field::value::type::string:
                    length_required = true;
                    break;
                    
                case kdk::assembler::field::value::type::color:
                    value_size = 4;
                    break;
                    
                case kdk::assembler::field::value::type::bitmask:
                    size_required = true;
                    break;
                    
                default:
                    break;
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

static inline void parse_symbol_list(kdl::sema *sema, kdk::assembler::field::value& value)
{
    std::vector<std::tuple<std::string, std::string>> symbols;
    
    sema->ensure({ kdl::condition(kdl::lexer::token::type::lbrace).truthy() });
    
    while (sema->expect({ kdl::condition(kdl::lexer::token::type::rbrace).falsey() })) {
        // Get the name of the symbol
        if (sema->expect({ kdl::condition(kdl::lexer::token::type::identifier).falsey() })) {
            log::error(sema->peek().file(), sema->peek().line(), "Symbol name should be an identifier.");
        }
        auto symbol_name = sema->read().text();
        
        sema->ensure({ kdl::condition(kdl::lexer::token::type::equals).truthy() });
        
        // Get the value of the symbol. These are _always_ integers.
        if (sema->expect({ kdl::condition(kdl::lexer::token::type::integer).falsey() })) {
            log::error(sema->peek().file(), sema->peek().line(), "Symbol value should be an integer.");
        }
        auto symbol_value = sema->read().text();
        
        sema->ensure({ kdl::condition(kdl::lexer::token::type::semi_colon).truthy() });
        
        symbols.push_back(std::make_tuple(symbol_name, symbol_value));
    }
    
    sema->ensure({ kdl::condition(kdl::lexer::token::type::rbrace).truthy() });
    
    value.set_symbols(symbols);
}

// MARK: - Parser

void kdl::define_directive::parse(kdl::sema *sema)
{
    std::string resource_type_name;
    std::string resource_type_code;
    std::vector<kdk::assembler::field> resource_fields;
    std::vector<kdk::assembler::reference> resource_references;
    
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
            std::string deprecation_note;
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
                else if (attribute_name == "deprecated") {
                    // Parse the deprecation note. This is a fixed format and does not change. No need
                    // for fancy stack based parsing.
                    if (sema->expect({
                        kdl::condition(kdl::lexer::token::type::lparen).truthy(),
                        kdl::condition(kdl::lexer::token::type::string).truthy(),
                        kdl::condition(kdl::lexer::token::type::rparen).truthy(),
                    })) {
                        sema->advance();
                        deprecation_note = sema->read().text();
                        sema->advance();
                    }
                    else {
                        log::error(sema->peek().file(), sema->peek().line(), "Invalid `deprecated()` format found.");
                    }
                }
                else if (attribute_name == "value") {
                    auto value = parse_field_value(sema);
                    
                    // Check if there is a symbol list attached.
                    if (sema->expect({ kdl::condition(kdl::lexer::token::type::lbrace).truthy() })) {
                        parse_symbol_list(sema, value);
                    }
                    
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
                    .set_deprecation_note(deprecation_note)
                    .set_required(required)
			);
            
        }
        else if (item_name == "reference") {
            // Add a new field into the resource type.
            // The syntax is:
            //  reference(reference_name) { args }
            
            auto reference_name = parse_field_name(sema);
            std::string type_name;
            std::vector<std::tuple<char, std::string>> id_map_operations;
            std::string lower_bound;
            std::string upper_bound;
            
            sema->ensure({
                kdl::condition(kdl::lexer::token::type::lbrace).truthy()
            });
            
            // Loop until we find the terminating r-brace.
            while (sema->expect({ kdl::condition(kdl::lexer::token::type::rbrace).falsey() })) {
                // All reference attributes start with an identifier.
                if (sema->expect({ kdl::condition(kdl::lexer::token::type::identifier).falsey() })) {
                    log::error(sema->peek().file(), sema->peek().line(), "Type definition reference attribute should start with an identifier");
                }
                auto attribute_name = sema->read().text();
                
                sema->ensure({
                    kdl::condition(kdl::lexer::token::type::equals).truthy()
                });
                
                if (attribute_name == "type") {
                    if (sema->expect({ kdl::condition(kdl::lexer::token::type::string).truthy() })) {
                        type_name = sema->read().text();
                    }
                    else {
                        log::error(sema->peek().file(), sema->peek().line(), "Invalid reference type name. Expected a string.");
                    }
                }
                else if (attribute_name == "valid_id_range") {
                    // The valid id range accepts two resource id's, which represent a lower and upper bound on the
                    // resources that can be produced.
                    if (sema->expect({
                        kdl::condition(kdl::lexer::token::type::resource_id).truthy(),
                        kdl::condition(kdl::lexer::token::type::resource_id).truthy(),
                    })) {
                        lower_bound = sema->read().text();
                        upper_bound = sema->read().text();
                    }
                    else {
                        log::error(sema->peek().file(), sema->peek().line(), "Invalid resource id range provided. Expected two resource ids.");
                    }
                }
                else if (attribute_name == "id_mapping") {
                    // The id mapping accepts a repeating pattern of variables, numbers and arithmetic symbols (+, -, * /). We
                    // keep iterating until we find a ';' or an invalid token.
                    char current_operator = '+';
                    
                    while (sema->expect({ kdl::condition(kdl::lexer::token::type::semi_colon).falsey() })) {
                        // Check if the token is an ID variable
                        if (sema->expect({ kdl::condition(kdl::lexer::token::type::variable, "id").truthy() })) {
                            id_map_operations.push_back(std::make_tuple(current_operator, sema->read().text()));
                        }
                        // Check if the token is an integer
                        else if (sema->expect({ kdl::condition(kdl::lexer::token::type::integer).truthy() })) {
                            id_map_operations.push_back(std::make_tuple(current_operator, sema->read().text()));
                        }
                        else {
                            log::error(sema->peek().file(), sema->peek().line(), "Invalid token found inside id_mapping. Expected $id or integer.");
                        }
                        
                        // Check if the token is a plus
                        if (sema->expect({ kdl::condition(kdl::lexer::token::type::plus).truthy() })) {
                            sema->advance();
                            current_operator = '+';
                        }
                        // Check if the token is a minus
                        else if (sema->expect({ kdl::condition(kdl::lexer::token::type::minus).truthy() })) {
                            sema->advance();
                            current_operator = '-';
                        }
                        // Check if the token is a star (multiply)
                        else if (sema->expect({ kdl::condition(kdl::lexer::token::type::star).truthy() })) {
                            sema->advance();
                            current_operator = '*';
                        }
                        // Check if the token is a slash (divide)
                        else if (sema->expect({ kdl::condition(kdl::lexer::token::type::slash).truthy() })) {
                            sema->advance();
                            current_operator = '/';
                        }
                        // Check if the token is a semi-colon
                        else if (sema->expect({ kdl::condition(kdl::lexer::token::type::semi_colon).truthy() })) {
                            break;
                        }
                        // Invalid operator token encountered
                        else {
                            log::error(sema->peek().file(), sema->peek().line(), "Invalid operator token found inside id_mapping. Expected +, -, * or /.");
                        }
                    }
                    
                }
                
                sema->ensure({ kdl::condition(kdl::lexer::token::type::semi_colon).truthy() });
            }
            
            sema->ensure({
                kdl::condition(kdl::lexer::token::type::rbrace).truthy()
            });
            
            // Finished parsing the reference definition...
            resource_references.push_back(
				kdk::assembler::reference(reference_name)
                    .set_id_mapping(id_map_operations)
                    .set_type(type_name)
                    .set_id_range(std::stoll(lower_bound), std::stoll(upper_bound))
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
    
    // Construct the type assembler and register it into main assembler.
    auto assembler = std::make_shared<kdk::assembler>();
    for (auto field : resource_fields) {
        assembler->add_field(field);
    }
    for (auto reference : resource_references) {
        assembler->add_reference(reference);
    }
    kdk::assembler_pool::shared().register_assembler(resource_type_name, resource_type_code, assembler);
    
}
