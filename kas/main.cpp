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
    // Check if any arguments have been supplied. If none have been supplied then display an error
    // page.
    if (option_exists(argv, argv + argc, "--help") || option_exists(argv, argv + argc, "-h")) {
        std::cout   << "The Kestrel Assembler -- Version 0.2" << std::endl
                    << "    kas [options] input_file ..." << std::endl << std::endl
                    << "Multiple files added to the build will be included into the same output file." << std::endl << std::endl
                    << "Options" << std::endl
                    << "  --scenario        The scenario definition files to assemble against." << std::endl
                    << "  --format          The output data format to be assembled. Should be 'classic', 'extended' or 'rez'." << std::endl
                    << "  -o                The destination file for the assembled data to be written to." << std::endl
                    << "  -h, --help        Display this help message." << std::endl;
        return 0;
    }

    // Step through all of the arguments and determine where the _first_ input file is located.
    std::string scenario_path { "" };
    std::string output_file { "plugin.kdat" };
    std::vector<std::string> input_files;

    for (auto i = 1; i < argc; ++i) {
        // If the argument starts with a '-' then skip it. If the argument depends on additional arguments, then
        // skip those as well.
        if (argv[i][0] != '-') {
            input_files.push_back(argv[i]);
            continue;
        }

        std::string option { argv[i] };
        if (option == "--scenario" && i < argc - 1) {
            scenario_path = std::string(argv[++i]);
        }
        else if (option == "--format" && i < argc - 1) {
            std::string format { argv[++i] };
        }
        else if (option == "-o" && i < argc - 1) {
            output_file = std::string(argv[++i]);
        }
        else {
            std::cout << "kas: \x1b[31merror: \x1b[0mbad argument supplied: " << option << std::endl;
            return 2;
        }
    }

    // Setup a new target.
    auto target = std::make_shared<kdk::target>(output_file);

    // Iterate through each of the input files.
    for (auto file : input_files) {
        auto lexer = kdl::lexer::open_file(file);
        auto sema = kdl::sema(target, lexer.analyze());
        sema.run();
    }

    target->build();

    return 0;
}
