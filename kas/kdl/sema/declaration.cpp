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
    
    sema->ensure({
        condition(lexer::token::type::lbrace).truthy()
    });
    
    // The general structure of declarations is consistant between StructureTypes.
    // This should allow us to parse out a set of resource instances
    while (sema->expect({ condition(lexer::token::type::rbrace).falsey() })) {
        
        // An instance of resource is denoted by the "new" keyword.
        
        if (sema->expect({ condition(lexer::token::type::identifier, "new").truthy() })) {
            parse_instance(sema);
        }
        
    }
}

void kdl::declaration::parse_instance(kdl::sema *sema)
{
    sema->ensure({
        condition(lexer::token::type::identifier, "new").truthy(),
        condition(lexer::token::type::lparen).truthy()
    });
    
    int64_t resource_id { 0 };
    std::string resource_name { "" };
    
    // We need to parse attributes until we hit a closing parentheses.
    while (sema->expect(condition(lexer::token::type::rparen).falsey())) {
        
        if (sema->expect({ condition(lexer::token::type::identifier).falsey(), condition(lexer::token::type::equals).falsey() })) {
            throw std::runtime_error("Malformed resource attribute encountered.");
        }
        auto attribute = sema->read().text();
        sema->advance();
        
        if (attribute == "id") {
            // We're expecting a resource id now.
            if (sema->expect({ condition(lexer::token::type::resource_id).falsey() })) {
                throw std::runtime_error("The id attribute must be assigned a resource id literal.");
            }
            resource_id = std::stoi(sema->read().text());
        }
        else if (attribute == "name") {
            // We're expecting a string now.
            if (sema->expect({ condition(lexer::token::type::string).falsey() })) {
                throw std::runtime_error("The name attribute must be assigned a string literal.");
            }
            resource_name = sema->read().text();
        }
        else {
            // Unrecognised attribute.
            throw std::runtime_error("Unrecognised resource attribute '" + attribute + "' encountered.");
        }
        
        // Check for a comma. If no comma exists, then we require the presence of a rparen.
        if (sema->expect({ condition(lexer::token::type::comma).truthy() })) {
            sema->advance();
            continue;
        }
        
        std::cout << "New resource #" << std::to_string(resource_id) << " '" << resource_name << "'" << std::endl;
        
        sema->ensure({ condition(lexer::token::type::rparen).truthy() });
        break;
        
    }
}
