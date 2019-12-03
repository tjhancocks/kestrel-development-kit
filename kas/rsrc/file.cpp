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

#include "rsrc/file.hpp"

// MARK: - Constructor

rsrc::file::file(const std::string path)
    : m_path(path) 
{
    
}


// MARK: - Resource Management

void rsrc::file::add_resource(const std::string type, int64_t id, const std::string name, rsrc::data data)
{
    // Get the container
    auto container = get_type_container(type);
    if (!container) {
        throw std::runtime_error("Failed to find or create resource type container for " + type);
    }
    
    // Add the resource...
    auto resource = rsrc::file::resource::create(id, name, data);
    container->add_resource(resource);
}

std::shared_ptr<rsrc::file::type_container> rsrc::file::get_type_container(const std::string type_code)
{
    // Try and find the container first...
    for (auto container : m_containers) {
        if (container->type_code() == type_code) {
            return container;
        }
    }
    
    // Failed to find, so create it...
    auto container = rsrc::file::type_container::create(type_code);
    m_containers.push_back(container);
    return container;
}


// MARK: - Resource

rsrc::file::resource::resource(int64_t id, const std::string name, rsrc::data blob)
    : m_id(id), m_name(name), m_blob(blob)
{
    
}

std::shared_ptr<rsrc::file::resource> rsrc::file::resource::create(int64_t id, const std::string name, rsrc::data blob)
{
    return std::make_shared<rsrc::file::resource>(id, name, blob);
}

int64_t rsrc::file::resource::id() const
{
    return m_id;
}

std::string rsrc::file::resource::name() const
{
    return m_name;
}

// MARK: - Type Container

rsrc::file::type_container::type_container(const std::string type_code)
    : m_code(type_code)
{
    
}

std::shared_ptr<rsrc::file::type_container> rsrc::file::type_container::create(const std::string type_code)
{
    return std::make_shared<rsrc::file::type_container>(type_code);
}

void rsrc::file::type_container::add_resource(std::shared_ptr<rsrc::file::resource> resource)
{
    m_resources.push_back(resource);
}

std::string rsrc::file::type_container::type_code() const
{
    return m_code;
}
