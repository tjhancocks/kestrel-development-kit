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
#include "kdl/sema/directive.hpp"
#include "kdl/sema/define_directive.hpp"
#include "diagnostic/log.hpp"
#include "kdl/lexer.hpp"

// MARK: - Parser

bool kdl::directive::test(kdl::sema *sema)
{
    return sema->expect(condition(kdl::lexer::token::type::directive).truthy());
}


void kdl::directive::parse(kdl::sema *sema)
{
    // Ensure directive.
    if (sema->expect(condition(kdl::lexer::token::type::directive).falsey())) {
        auto tk = sema->peek();
        log::error(tk.file(), tk.line(), "Unexpected token '" + tk.text() + "' encountered while parsing directive.");
    }
    
    // Directive structure: @directive { <args> }
    auto directive = sema->read().text();
    
    if (sema->expect(condition(kdl::lexer::token::type::lbrace).falsey())) {
        auto tk = sema->peek();
        log::error(tk.file(), tk.line(), "Expected '{' whilst starting directive, but found '" + tk.text() + "' instead.");
    }
    sema->advance();
    
    if (directive == "out") {
        // Consume each of the arguments.
        auto args = sema->consume(condition(kdl::lexer::token::type::rbrace).falsey());
        
        // The `@out` directive prints to the standard output.
        for (auto a : args) {
            std::cout << a.text() << std::endl;
        }
    }
    else if (directive == "define") {
        // Defines a new resource type for the assembler to use. This is a complex operation,
        // so hand off to another function.
        kdl::define_directive::parse(sema);
    }
    else if (directive == "import") {
        // Consume each of the arguments.
        auto args = sema->consume(condition(kdl::lexer::token::type::rbrace).falsey());
        
        // The `@import` directive imports the contents of another file and inserts it
        // into the current token stream.
        for (auto a : args) {
            auto lexer = kdl::lexer::open_file(a.text());
            auto tokens = lexer.analyze();
            sema->insert_tokens(tokens);
        }
    }
    else {
        log::error(sema->peek().file(), sema->peek().line(), "Unknown directive @" + directive);
    }
    
    if (sema->expect(condition(kdl::lexer::token::type::rbrace).falsey())) {
        auto tk = sema->peek();
        log::error(tk.file(), tk.line(), "Expected '}' whilst finishing directive, but found '" + tk.text() + "' instead.");
    }
    sema->advance();
}
