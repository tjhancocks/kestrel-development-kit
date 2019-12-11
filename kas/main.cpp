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

#include <string>
#include <iostream>
#include <algorithm>
#include "kdl/lexer.hpp"
#include "kdl/sema.hpp"

// MARK: - Command Line Helpers

bool option_exists(const char **begin, const char **end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

const char *get_option(const char **begin, const char **end, const std::string& option)
{
    const char **i = std::find(begin, end, option);
    if (i != end && ++i != end) {
        return *i;
    }
    return nullptr;
}

// MARK: - Entry Point

int main(int argc, const char **argv)
{
    
    // Check if any arguments have been supplied.
    if (option_exists(argv, argv + argc, "--help") || option_exists(argv, argv + argc, "-h")) {
        std::cout   << "The Kestrel Assembler -- Version 0.1" << std::endl
                    << "    kas [options] -f input_file" << std::endl << std::endl
                    << "Options" << std::endl
                    << "  -f,               The KDL source file to assemble." << std::endl
                    << "  -o                The destination KDAT file for the assembled data file to be written to." << std::endl
                    << "  -h, --help        Display this help message." << std::endl;
        return 0;
    }
    
    std::string input_file { "" };
    if (option_exists(argv, argv + argc, "-f")) {
        input_file = get_option(argv, argv + argc, "-f");
    }
    
    std::string output_file { "kestrel-plugin.kdat" };
    if (option_exists(argv, argv + argc, "-o")) {
        output_file = get_option(argv, argv + argc, "-o");
    }
    
    
    if (argc <= 1 || input_file.empty()) {
        std::cout << "kas: \x1b[31merror: \x1b[0mno input file" << std::endl;
        return 1;
    }   
    
    // Perform the workflow (lexical analysis, semantic analysis...)
    auto lexer = kdl::lexer::open_file(input_file);
    auto sema = kdl::sema(kdk::target(output_file), lexer.analyze());
    sema.run();
    
    // The semantic analysis should have resulted in a completed target structure, which
    // can now be assembled.
    sema.target().build();
    
    return 0;
}
