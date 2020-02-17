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
#include "kdl/sema/declaration.hpp"
#include "structures/resource.hpp"
#include "diagnostic/log.hpp"
#include "assemblers/pool.hpp"

// MARK: - Parser

bool kdl::declaration::test(kdl::sema *sema)
{
    return sema->expect({
        kdl::condition(lexer::token::type::identifier, "declare").truthy(),
        kdl::condition(lexer::token::type::identifier).truthy(),
        kdl::condition(lexer::token::type::lbrace).truthy()
    });
}


void kdl::declaration::parse(kdl::sema *sema)
{
    // Ensure declaration.
    sema->ensure({
        condition(lexer::token::type::identifier, "declare").truthy()
    });
    
    // Declaration structure: declare StructureName { <args> }
    auto structure_name = sema->read().text();
    std::vector<kdk::resource> m_instances;
    
    sema->ensure({
        condition(lexer::token::type::lbrace).truthy()
    });
    
    // The general structure of declarations is consistant between StructureTypes.
    // This should allow us to parse out a set of resource instances
    while (sema->expect({ condition(lexer::token::type::rbrace).falsey() })) {
        
        // An instance of resource is denoted by the "new" keyword.
        
        if (sema->expect({ condition(lexer::token::type::identifier, "new").truthy() })) {
            m_instances.push_back(parse_instance(sema, structure_name));
        }
        
    }
    
    sema->ensure({
        condition(lexer::token::type::rbrace).truthy()
    });
    
    // Prepare to add the structure type and all instances into the target.
    sema->target()->add_resources(m_instances);
}

kdk::resource kdl::declaration::parse_instance(kdl::sema *sema, const std::string type, bool ignore_attributes, int64_t default_id, std::string default_name)
{
    sema->ensure({
        condition(lexer::token::type::identifier, "new").truthy()
    });
    
    int64_t resource_id { default_id };
    std::string resource_name { default_name };
    
    if (!ignore_attributes) {
        sema->ensure({
            condition(lexer::token::type::lparen).truthy()
        });

        // We need to parse attributes until we hit a closing parentheses.
        while (sema->expect(condition(lexer::token::type::rparen).falsey())) {
            
            if (sema->expect({ condition(lexer::token::type::identifier).falsey(), condition(lexer::token::type::equals).falsey() })) {
                log::error(sema->peek().file(), sema->peek().line(), "Malformed resource attribute encountered.");
            }
            auto attribute = sema->read().text();
            sema->advance();
            
            if (attribute == "id") {
                // We're expecting a resource id now.
                if (sema->expect({ condition(lexer::token::type::resource_id).falsey() })) {
                    log::error(sema->peek().file(), sema->peek().line(), "The 'id' attribute must be assigned a resource id literal.");
                }
                resource_id = std::stoi(sema->read().text());
            }
            else if (attribute == "name") {
                // We're expecting a string now.
                if (sema->expect({ condition(lexer::token::type::string).falsey() })) {
                    log::error(sema->peek().file(), sema->peek().line(), "The 'name' attribute must be assigned a string literal.");
                }
                resource_name = sema->read().text();
            }
            else {
                // Unrecognised attribute.
                log::error(sema->peek().file(), sema->peek().line(), "Unrecognised resource attribute '" + attribute + "' encountered.");
            }
            
            // Check for a comma. If no comma exists, then we require the presence of a rparen.
            if (sema->expect({ condition(lexer::token::type::comma).truthy() })) {
                sema->advance();
                continue;
            }
            
            sema->ensure({ condition(lexer::token::type::rparen).truthy() });
            break;
            
        }
    }
    
    // Construct the base resource object in preparation for adding fields and values to it.
    kdk::resource resource { type, resource_id, resource_name };
    
    // All fields are contained with in a block ( { ... } ). Ensure we have an opening brace, and then keep
    // parsing until the corresponding closing brace is found.
    sema->ensure({
        condition(lexer::token::type::lbrace).truthy()
    });
    
    while ( sema->expect({ condition(lexer::token::type::rbrace).falsey() }) ) {
        
        // Resource fields have the following grammar:
        //  field_name = value...;
        //
        // In this context, each value may be one of a couple of different grammars
        //
        //      string
        //      resource_id
        //      integer
        //      percentage
        //      identifier
        //      identifier<file> ( string )
        //
        // Each of these need to be correctly parsed and encoded into a field structure
        // and stored in the resource. This part of the parser is not validing the valuetypes
        // for the fields.
        // There can be one or more values, and values are consumed until a semi-colon is
        // found. There _must_ be at least one value provided.
        
        if ( sema->expect({ condition(lexer::token::type::identifier).falsey() })) {
            log::error(sema->peek().file(), sema->peek().line(), "Resource field name must be an identifier.");
        }
        auto field_name = sema->read().text();
        
        sema->ensure({
            condition(lexer::token::type::equals).truthy()
        });
        
        if (sema->expect({ kdl::condition(kdl::lexer::token::type::identifier, "new").truthy() })) {
            // We're trying to construct a referenced resource. Ensure that the field_name specified correlates to a reference
            // in the resource definition.
            auto assembler_info = kdk::assembler_pool::shared().assembler_named(type);
            auto assembler = std::get<1>(assembler_info);
            if (assembler == nullptr) {
                log::error(sema->peek().file(), sema->peek().line(), "Unable to handle referenced resource declaration. Unable to identify it.");
            }
            
            auto reference = assembler->find_reference_definition(field_name);
            if (reference == nullptr) {
                log::error(sema->peek().file(), sema->peek().line(), "Unable to handle referenced resource declaration. Missing definition.");
            }
            
            // Prepare to calculate the ID for the referenced resource.
            int64_t reference_reference_id = 0;
            for (auto op : reference->id_map_operations()) {
                int64_t value = 0;
                if (std::get<1>(op) == "id") {
                    value = resource_id;
                }
                else {
                    value = std::stoll(std::get<1>(op));
                }
                
                switch (std::get<0>(op)) {
                    case '+':
                        reference_reference_id += value;
                        break;
                    case '-':
                        reference_reference_id -= value;
                        break;
                    case '*':
                        reference_reference_id *= value;
                        break;
                    case '/':
                        reference_reference_id /= value;
                        break;
                    default:
                        log::error(sema->peek().file(), sema->peek().line(), "Unsupport ID Mapping operation attempted.");
                        break;
                }
            }
            
            // Parse the nested instance, to produce a new resource instance. Add that directly to the target.
            auto instance = parse_instance(sema, reference->type(), true, reference_reference_id, resource_name);
            sema->target()->add_resources({ instance });
        }
        else {
            // We're simply handling a field within the resource.
            std::vector<std::tuple<std::string, kdk::resource::field::value_type>> values { 0 };
            
            while ( sema->expect({ condition(lexer::token::type::semi_colon).falsey() }) ) {
                
                // Validate the value token.
                if ( sema->expect({ condition(lexer::token::type::string).truthy() }) ) {
                    // String value...
                    values.push_back( std::make_tuple(sema->read().text(), kdk::resource::field::value_type::string) );
                }
                else if ( sema->expect({ condition(lexer::token::type::integer).truthy() }) ) {
                    // Integer value...
                    values.push_back( std::make_tuple(sema->read().text(), kdk::resource::field::value_type::integer) );
                }
                else if ( sema->expect({ condition(lexer::token::type::percentage).truthy() }) ) {
                    // Percentage value...
                    values.push_back( std::make_tuple(sema->read().text(), kdk::resource::field::value_type::percentage) );
                }
                else if ( sema->expect({ condition(lexer::token::type::resource_id).truthy() }) ) {
                    // Resource ID value...
                    values.push_back( std::make_tuple(sema->read().text(), kdk::resource::field::value_type::resource_id) );
                }
                else if ( sema->expect({ condition(lexer::token::type::identifier, "file").truthy() }) ) {
                    // File reference value...
                    sema->ensure({
                        condition(lexer::token::type::identifier, "file").truthy(),
                        condition(lexer::token::type::lparen).truthy()
                    });
                    
                    if (sema->expect({ condition(lexer::token::type::string).falsey(), condition(lexer::token::type::rparen).falsey() })) {
                        log::error(sema->peek().file(), sema->peek().line(), "Malformed file reference found.");
                    }
                    
                    values.push_back( std::make_tuple(sema->read().text(), kdk::resource::field::value_type::file_reference) );
                    sema->advance();
                }
                else if ( sema->expect({ condition(lexer::token::type::identifier, "rgb").truthy() }) ) {
                    // RGB Color value...
                    sema->ensure({
                        condition(lexer::token::type::identifier, "rgb").truthy(),
                        condition(lexer::token::type::lparen).truthy()
                    });
                    
                    if (sema->expect({
                        condition(lexer::token::type::integer).falsey(),
                        condition(lexer::token::type::integer).falsey(),
                        condition(lexer::token::type::integer).falsey(),
                        condition(lexer::token::type::rparen).falsey()
                    })) {
                        log::error(sema->peek().file(), sema->peek().line(), "Malformed RGB color found.");
                    }
                    
                    auto red = static_cast<uint8_t>(std::stoi(sema->read().text()));
                    auto green = static_cast<uint8_t>(std::stoi(sema->read().text()));
                    auto blue = static_cast<uint8_t>(std::stoi(sema->read().text()));
                    uint32_t rgb = (red << 16) | (green << 8) | (blue);
                    
                    values.push_back( std::make_tuple(std::to_string(rgb), kdk::resource::field::value_type::color) );
                    sema->advance();
                }
                else if ( sema->expect({ condition(lexer::token::type::identifier).truthy() }) ) {
                    // Identifier reference...
                    values.push_back( std::make_tuple(sema->read().text(), kdk::resource::field::value_type::identifier) );
                }
                else {
                    log::error(sema->peek().file(), sema->peek().line(), "Unexpected value type encountered.");
                }
                
            }
            
            kdk::resource::field field { field_name, values };
            resource.add_field(field);
        }
        
        sema->ensure({
            condition(lexer::token::type::semi_colon).truthy()
        });
    }
    
    sema->ensure({
        condition(lexer::token::type::rbrace).truthy()
    });
    
    return resource;
}
