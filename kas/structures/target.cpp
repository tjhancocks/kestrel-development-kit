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

#include "structures/target.hpp"
#include "rsrc/file.hpp"
#include "types/sprite_animation.hpp"

// MARK: - Constructor

kdk::target::target(std::string path)
    : m_path(path)
{
    
}

// MARK: - Resource Management

void kdk::target::add_resources(const std::vector<kdk::resource> resources)
{
    m_resources.insert(std::end(m_resources), std::begin(resources), std::end(resources));
}

// MARK: - Build

void kdk::target::build()
{
    auto rf = rsrc::file::create(m_path);
    
    // Iterate through each of the resources and construct the data for each of them.
    // Currently this is done by manually testing the type and invoking the appropriate
    // type assembler. This could be improved by making KDL more extensible.
    for (auto resource : m_resources) {
        rsrc::data resource_data;
        std::string type_code;
        auto type = resource.type();
        
        if (type == "SpriteAnimation") {
            type_code = "spïn";
            resource_data = kdk::sprite_animation(resource, rf).assemble();
        }
        
        rf->add_resource(type_code, resource.id(), resource.name(), resource_data);
    }
    
    // The resource file should be assembled at this point and just needs writting to disk.
    return;
    
}
