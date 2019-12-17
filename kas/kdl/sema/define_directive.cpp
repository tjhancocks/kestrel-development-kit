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

// MARK: - Parser

void kdl::define_directive::parse(kdl::sema *sema)
{
    std::string resource_type_name;
    std::string resource_type_code;
    
    auto file = sema->peek().file();
    auto line = sema->peek().line();
    
    // Keep going until we encounter the closing brace.
    while (sema->expect({ kdl::condition(kdl::lexer::token::type::rbrace).falsey() })) {
        
        // All items in the directive start with an identifier. Check what the identifier
        // is in order to determine the course of action.
        auto item_name = sema->read().text();
        
        if (item_name == "name") {
            // Specify the name of the directive.
            sema->ensure({ kdl::condition(kdl::lexer::token::type::equals).truthy() });
            
            // Get the value of the item.
            if (sema->expect({ kdl::condition(kdl::lexer::token::type::string).falsey() })) {
                log::error(sema->peek().file(), sema->peek().line(), "Type definition name must be a string.");
            }
            resource_type_name = sema->read().text();
        }
        else if (item_name == "code") {
            // Specify the name of the directive.
            sema->ensure({ kdl::condition(kdl::lexer::token::type::equals).truthy() });
            
            // Get the value of the item.
            if (sema->expect({ kdl::condition(kdl::lexer::token::type::string).falsey() })) {
                log::error(sema->peek().file(), sema->peek().line(), "Type definition code must be a string.");
            }
            resource_type_code = sema->read().text();
        }
        else if (item_name == "field") {
            // Add a new field into the resource type.
            // The syntax is:
            //  field(field-name) { args }
            sema->ensure({ kdl::condition(kdl::lexer::token::type::lparen).truthy() });
            
            if (sema->expect({ kdl::condition(kdl::lexer::token::type::string).falsey() })) {
                log::error(sema->peek().file(), sema->peek().line(), "Type definition field name should be a string.");
            }
            auto field_name = sema->read().text();
            
            sema->ensure({
                kdl::condition(kdl::lexer::token::type::rparen).truthy(),
                kdl::condition(kdl::lexer::token::type::lbrace).truthy()
            });
            
            // We're now inside the context of the field. Pass on to another function
            // to parse this.
            
            // TODO
            
            sema->ensure({
                kdl::condition(kdl::lexer::token::type::rbrace).truthy()
            });
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
    
    std::cout << "Define resource type: " << resource_type_name << " '" << resource_type_code << "'" << std::endl;
}
