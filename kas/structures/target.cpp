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
    
    for (auto resource : m_resources) {
        auto type = resource.type();
        
        if (type == "SpriteAnimation") {
            auto data = kdk::sprite_animation(resource, rf).assemble();
        }
        
    }
    
    
}
